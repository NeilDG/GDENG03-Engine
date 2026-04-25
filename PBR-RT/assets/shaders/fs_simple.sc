$input v_worldPos, v_normal, v_texcoord0, v_color0

#include <bgfx_shader.sh>

// PBR uniforms
uniform vec4 u_lightDir;      // xyz: direction, w: intensity
uniform vec4 u_baseColor;     // rgba: albedo color
uniform vec4 u_pbrParams;     // x: metallic, y: roughness, z: ao, w: unused
uniform vec4 u_cameraPos;     // xyz: camera world position, w: unused
uniform vec4 u_enableIBL;     // x: 0.0 = disabled, 1.0 = enabled
samplerCube u_envMap;         // Raw environment (for skybox/fallback)
samplerCube u_irradianceMap;  // Precomputed irradiance for diffuse IBL
samplerCube u_prefilterMap;   // Prefiltered environment for specular IBL (with mips)

#define PI 3.14159265359

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
    // Extract PBR parameters
    float metallic = u_pbrParams.x;
    float roughness = u_pbrParams.y;
    float ao = u_pbrParams.z;

    // Ensure roughness is not zero to avoid singularities
    roughness = max(roughness, 0.04);

    // Normalize interpolated normal
    vec3 N = normalize(v_normal);

    // View direction (from surface to camera)
    vec3 V = normalize(u_cameraPos.xyz - v_worldPos);

    // Light direction (negate because u_lightDir points FROM light)
    vec3 L = normalize(-u_lightDir.xyz);

    // Half-vector between view and light
    vec3 H = normalize(V + L);

    // Calculate F0 (surface reflection at zero incidence)
    // Dielectrics have F0 around 0.04, metals use albedo color
    vec3 F0 = vec3_splat(0.04);
    F0 = mix(F0, u_baseColor.rgb, metallic);

    // Cook-Torrance BRDF
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
    vec3 diffuse = kD * u_baseColor.rgb / PI;

    // Calculate radiance
    float NdotL = max(dot(N, L), 0.0);
    float lightIntensity = u_lightDir.w;
    vec3 radiance = vec3_splat(lightIntensity);

    // Outgoing radiance Lo
    vec3 Lo = (diffuse + specular) * radiance * NdotL;

    // IBL ambient/specular
    vec3 ambient;
    if (u_enableIBL.x > 0.5) {
        // Use prefiltered maps for physically accurate IBL

        // Diffuse IBL: Sample irradiance map
        vec3 irradiance = textureCube(u_irradianceMap, N).rgb;
        vec3 diffuseIBL = irradiance * u_baseColor.rgb;

        // Specular IBL: Sample prefiltered environment map at roughness mip level
        vec3 R = reflect(-V, N);
        const float MAX_REFLECTION_LOD = 4.0; // 5 mip levels (0-4)
        float lod = roughness * MAX_REFLECTION_LOD;
        vec3 prefilteredColor = textureCubeLod(u_prefilterMap, R, lod).rgb;

        // Fresnel for IBL
        vec3 F_ibl = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

        // Energy conservation for IBL
        vec3 kS_ibl = F_ibl;
        vec3 kD_ibl = vec3_splat(1.0) - kS_ibl;
        kD_ibl *= 1.0 - metallic;

        // Combine diffuse and specular IBL
        vec3 diffuse_ambient = kD_ibl * diffuseIBL;
        vec3 specular_ambient = kS_ibl * prefilteredColor;

        ambient = (diffuse_ambient + specular_ambient) * ao;
    } else {
        // Fallback: simple ambient without IBL
        vec3 kS_ambient = fresnelSchlick(max(dot(N, V), 0.0), F0);
        vec3 kD_ambient = vec3_splat(1.0) - kS_ambient;
        kD_ambient *= 1.0 - metallic;
        ambient = (kD_ambient * u_baseColor.rgb + kS_ambient * F0) * ao * 0.6;
    }

    // Final color
    vec3 color = ambient + Lo;

    // Tone mapping (Reinhard)
    color = color / (color + vec3_splat(1.0));

    // Gamma correction (assuming sRGB output)
    color = pow(color, vec3_splat(1.0/2.2));

    gl_FragColor = vec4(color, u_baseColor.a);
}
