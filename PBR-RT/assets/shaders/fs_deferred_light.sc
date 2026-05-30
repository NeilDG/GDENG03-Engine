// Deferred Lighting Fragment Shader
// Reads G-Buffer textures and applies PBR lighting (Cook-Torrance BRDF + IBL)
// This is the "lighting pass" of deferred rendering
// Reference: BGFX Example 21-deferred, Filament PBR, Real-Time Rendering 4th Ed. Ch. 20

$input v_texcoord0

#include <bgfx_shader.sh>
#include "include/pbr_common.sh"

// G-Buffer texture samplers (must match AnitoGBuffer layout)
SAMPLER2D(s_gbuffer0, 0);  // RT0: Albedo (RGB) + Metallic (A)
SAMPLER2D(s_gbuffer1, 1);  // RT1: Normal (RGB, encoded [0,1]) + Roughness (A)
SAMPLER2D(s_gbuffer2, 2);  // RT2: World Position (RGB) + AO (A)
SAMPLER2D(s_gbuffer3, 3);  // RT3: Emission (RGB) + Unused (A)

// Lighting uniforms (same as forward rendering)
uniform vec4 u_lightDir;      // xyz: direction, w: intensity
uniform vec4 u_cameraPos;     // xyz: camera world position, w: unused
uniform vec4 u_enableIBL;     // x: 0.0 = disabled, 1.0 = enabled

// IBL cubemap samplers — use bgfx SAMPLERCUBE macro so binding slots match C++ side
SAMPLERCUBE(s_irradianceMap, 4);  // Precomputed irradiance for diffuse IBL
SAMPLERCUBE(s_prefilterMap,  5);  // Prefiltered environment for specular IBL (with mips)
SAMPLER2D(s_brdfLUT, 6);          // BRDF integration LUT (split-sum)

#define PI 3.14159265359

// Helper function: Decode normal from [0, 1] to [-1, 1] range
vec3 decodeNormal(vec3 encoded) {
    return encoded * 2.0 - 1.0;  // Map [0,1] to [-1,1]
}

// Fresnel-Schlick with roughness for IBL
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3_splat(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // Sample G-Buffer textures
    vec4 gbuffer0 = texture2D(s_gbuffer0, v_texcoord0);  // Albedo + Metallic
    vec4 gbuffer1 = texture2D(s_gbuffer1, v_texcoord0);  // Normal + Roughness
    vec4 gbuffer2 = texture2D(s_gbuffer2, v_texcoord0);  // Position + AO
    vec4 gbuffer3 = texture2D(s_gbuffer3, v_texcoord0);  // Emission

    // Detect empty pixels (no geometry wrote to G-Buffer).
    // Output transparent to preserve previously rendered skybox.
    float geometrySignal = dot(abs(gbuffer0.rgb), vec3_splat(1.0))
        + dot(abs(gbuffer2.rgb), vec3_splat(1.0))
        + dot(abs(gbuffer3.rgb), vec3_splat(1.0))
        + gbuffer0.a + gbuffer1.a + gbuffer2.a;
    if (geometrySignal < 0.0001)
    {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    // Extract material properties from G-Buffer
    vec3 albedo = gbuffer0.rgb;
    float metallic = gbuffer0.a;

    vec3 normalEncoded = gbuffer1.rgb;
    float roughness = gbuffer1.a;

    vec3 worldPos = gbuffer2.rgb;
    float ao = gbuffer2.a;

    vec3 emission = gbuffer3.rgb;

    roughness = max(roughness, 0.04);

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
    vec3 specular = BRDF_Specular_GGX(N, V, L, F0, roughness);
    vec3 F = F_Schlick(max(dot(H, V), 0.0), F0);

    // Energy conservation: diffuse contribution
    vec3 kS = F;
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
        vec3 irradiance = textureCube(s_irradianceMap, N).rgb;
        irradiance = pow(abs(irradiance), vec3_splat(2.2)); // toLinear
        vec3 diffuseIBL = irradiance * albedo;

        vec3 R = reflect(-V, N);
        const float MAX_REFLECTION_LOD = 4.0; // 5 mip levels (0-4)
        float lod = roughness * MAX_REFLECTION_LOD;
        vec3 prefilteredColor = textureCubeLod(s_prefilterMap, R, lod).rgb;
        prefilteredColor = pow(abs(prefilteredColor), vec3_splat(2.2)); // toLinear

        float NoV = max(dot(N, V), 0.0);
        vec3 F_ibl = fresnelSchlickRoughness(NoV, F0, roughness);
        vec3 kS_ibl = F_ibl;
        vec3 kD_ibl = vec3_splat(1.0) - kS_ibl;
        kD_ibl *= 1.0 - metallic;

        float iblDiffuseStrength = 2.0;
        float iblSpecularStrength = 2.5;

        vec3 diffuse_ambient = kD_ibl * diffuseIBL * iblDiffuseStrength;

        // Advanced: Split-sum specular term using BRDF LUT
        vec2 brdf = texture2D(s_brdfLUT, vec2(NoV, roughness)).rg;
        vec3 specular_ambient = prefilteredColor * (F0 * brdf.x + brdf.y);

        specular_ambient *= iblSpecularStrength;
        ambient = (diffuse_ambient + specular_ambient) * ao;
    } else {
        // Fallback: simple ambient without IBL
        vec3 kS_ambient = F_Schlick(max(dot(N, V), 0.0), F0);
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
