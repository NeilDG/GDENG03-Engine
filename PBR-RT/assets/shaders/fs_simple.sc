$input v_worldPos, v_normal, v_texcoord0, v_color0

#include <bgfx_shader.sh>
#include "include/pbr_common.sh"

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
// Moved to shared include as F_Schlick for Phase 5 Step 4.

// Fresnel-Schlick with roughness for IBL
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3_splat(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX/Trowbridge-Reitz normal distribution function (Filament spec eq. 4)
// Moved to shared include as D_GGX for Phase 5 Step 2.

// Geometry function and Smith visibility moved to shared include (Phase 5 Step 3).

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
    vec3 specular = BRDF_Specular_GGX(N, V, L, F0, roughness);
    vec3 F = F_Schlick(max(dot(H, V), 0.0), F0);

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

        // Diffuse IBL: Sample irradiance map and convert to linear
        vec3 irradiance = textureCube(u_irradianceMap, N).rgb;
        irradiance = pow(abs(irradiance), vec3_splat(2.2)); // toLinear
        vec3 diffuseIBL = irradiance * u_baseColor.rgb;

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
        float iblDiffuseStrength = 2.0;  // Reduced from 3.5 (toLinear gives more accurate values)
        float iblSpecularStrength = 2.5; // Reduced from 4.0 (filmic tone mapping is less aggressive)

        vec3 diffuse_ambient = kD_ibl * diffuseIBL * iblDiffuseStrength;
        vec3 specular_ambient = kS_ibl * prefilteredColor * iblSpecularStrength;

        // Apply AO
        ambient = (diffuse_ambient + specular_ambient) * ao;
    } else {
        // Fallback: simple ambient without IBL
        vec3 kS_ambient = F_Schlick(max(dot(N, V), 0.0), F0);
        vec3 kD_ambient = vec3_splat(1.0) - kS_ambient;
        kD_ambient *= 1.0 - metallic;
        ambient = (kD_ambient * u_baseColor.rgb + kS_ambient * F0) * ao * 0.6;
    }

    // Combine direct lighting and ambient (IBL)
    vec3 color = Lo + ambient;

    // Filmic tone mapping (BGFX pattern - matches skybox)
    color = max(vec3_splat(0.0), color - 0.004);
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);

    gl_FragColor = vec4(color, u_baseColor.a);
}
