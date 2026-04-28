// Deferred Lighting Fragment Shader
// Reads G-Buffer textures and applies PBR lighting (Cook-Torrance BRDF + IBL)
// This is the "lighting pass" of deferred rendering
// Reference: BGFX Example 21-deferred, Filament PBR, Real-Time Rendering 4th Ed. Ch. 20

$input v_texcoord0

#include <bgfx_shader.sh>

// G-Buffer texture samplers (must match AnitoGBuffer layout)
SAMPLER2D(s_gbuffer0, 0);  // RT0: Albedo (RGB) + Metallic (A)
SAMPLER2D(s_gbuffer1, 1);  // RT1: Normal (RGB, encoded [0,1]) + Roughness (A)
SAMPLER2D(s_gbuffer2, 2);  // RT2: World Position (RGB) + AO (A)
SAMPLER2D(s_gbuffer3, 3);  // RT3: Emission (RGB) + Unused (A)

// Lighting uniforms (same as forward rendering)
uniform vec4 u_lightDir;      // xyz: direction, w: intensity
uniform vec4 u_cameraPos;     // xyz: camera world position, w: unused
uniform vec4 u_enableIBL;     // x: 0.0 = disabled, 1.0 = enabled

// IBL cubemap samplers
samplerCube u_envMap;         // Raw environment (for skybox/fallback)
samplerCube u_irradianceMap;  // Precomputed irradiance for diffuse IBL
samplerCube u_prefilterMap;   // Prefiltered environment for specular IBL (with mips)

#define PI 3.14159265359

// Helper function: Decode normal from [0, 1] to [-1, 1] range
vec3 decodeNormal(vec3 encoded) {
    return encoded * 2.0 - 1.0;  // Map [0,1] to [-1,1]
}

// Fresnel-Schlick approximation (Filament spec eq. 2)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Fresnel-Schlick with roughness for IBL
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3_splat(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX/Trowbridge-Reitz normal distribution function (Filament spec eq. 4)
float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / max(denom, 0.001);  // Prevent divide by zero
}

// Geometry function - Schlick-GGX (Filament spec eq. 8)
float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;

    return NdotV / max(denom, 0.001);
}

// Smith's method for geometry obstruction (Filament spec eq. 9)
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main()
{
    // Sample G-Buffer textures
    vec4 gbuffer0 = texture2D(s_gbuffer0, v_texcoord0);  // Albedo + Metallic
    vec4 gbuffer1 = texture2D(s_gbuffer1, v_texcoord0);  // Normal + Roughness
    vec4 gbuffer2 = texture2D(s_gbuffer2, v_texcoord0);  // Position + AO
    vec4 gbuffer3 = texture2D(s_gbuffer3, v_texcoord0);  // Emission

    // Extract material properties from G-Buffer
    vec3 albedo = gbuffer0.rgb;
    float metallic = gbuffer0.a;

    vec3 normalEncoded = gbuffer1.rgb;
    float roughness = gbuffer1.a;

    vec3 worldPos = gbuffer2.rgb;
    float ao = gbuffer2.a;

    vec3 emission = gbuffer3.rgb;

    // Decode normal from [0,1] to [-1,1] and renormalize
    vec3 N = normalize(decodeNormal(normalEncoded));

    // View direction (from surface to camera)
    vec3 V = normalize(u_cameraPos.xyz - worldPos);

    // Light direction (negate because u_lightDir points FROM light)
    vec3 L = normalize(-u_lightDir.xyz);

    // Half-vector between view and light
    vec3 H = normalize(V + L);

    // Calculate F0 (surface reflection at zero incidence)
    // Dielectrics have F0 around 0.04, metals use albedo color
    vec3 F0 = vec3_splat(0.04);
    F0 = mix(F0, albedo, metallic);

    // ===== DIRECT LIGHTING (Cook-Torrance BRDF) =====

    // Cook-Torrance BRDF components
    float NDF = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    // Specular contribution
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    // Energy conservation: diffuse contribution
    // kS is equal to Fresnel
    vec3 kS = F;
    // For energy conservation, diffuse and specular light can't exceed 1.0
    // Metallic surfaces have no diffuse lighting
    vec3 kD = vec3_splat(1.0) - kS;
    kD *= 1.0 - metallic;

    // Lambert diffuse
    vec3 diffuse = kD * albedo / PI;

    // Calculate radiance
    float NdotL = max(dot(N, L), 0.0);
    float lightIntensity = u_lightDir.w;
    vec3 radiance = vec3_splat(lightIntensity);

    // Outgoing radiance Lo (direct lighting)
    vec3 Lo = (diffuse + specular) * radiance * NdotL;

    // ===== IMAGE-BASED LIGHTING (IBL) =====

    vec3 ambient;
    if (u_enableIBL.x > 0.5) {
        // Use prefiltered maps for physically accurate IBL

        // Diffuse IBL: Sample irradiance map and convert to linear
        vec3 irradiance = textureCube(u_irradianceMap, N).rgb;
        irradiance = pow(abs(irradiance), vec3_splat(2.2)); // toLinear
        vec3 diffuseIBL = irradiance * albedo;

        // Specular IBL: Sample prefiltered environment map at roughness mip level
        vec3 R = reflect(-V, N);
        const float MAX_REFLECTION_LOD = 4.0; // 5 mip levels (0-4)
        float lod = roughness * MAX_REFLECTION_LOD;
        vec3 prefilteredColor = textureCubeLod(u_prefilterMap, R, lod).rgb;
        prefilteredColor = pow(abs(prefilteredColor), vec3_splat(2.2)); // toLinear

        // Fresnel for IBL
        vec3 F_ibl = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

        // Energy conservation for IBL
        vec3 kS_ibl = F_ibl;
        vec3 kD_ibl = vec3_splat(1.0) - kS_ibl;
        kD_ibl *= 1.0 - metallic;

        // Combine diffuse and specular IBL
        // Increased multipliers for indoor HDR maps (BGFX uses exp2(exposure))
        float iblDiffuseStrength = 2.0;  // Match forward rendering
        float iblSpecularStrength = 2.5; // Match forward rendering

        vec3 diffuse_ambient = kD_ibl * diffuseIBL * iblDiffuseStrength;
        vec3 specular_ambient = kS_ibl * prefilteredColor * iblSpecularStrength;

        // Apply AO
        ambient = (diffuse_ambient + specular_ambient) * ao;
    } else {
        // Fallback: simple ambient without IBL
        vec3 kS_ambient = fresnelSchlick(max(dot(N, V), 0.0), F0);
        vec3 kD_ambient = vec3_splat(1.0) - kS_ambient;
        kD_ambient *= 1.0 - metallic;
        ambient = (kD_ambient * albedo + kS_ambient * F0) * ao * 0.6;
    }

    // ===== FINAL COLOR COMPOSITION =====

    // Combine direct lighting, ambient (IBL), and emission
    vec3 color = Lo + ambient + emission;

    // Filmic tone mapping (BGFX pattern - matches forward rendering)
    color = max(vec3_splat(0.0), color - 0.004);
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);

    gl_FragColor = vec4(color, 1.0);
}
