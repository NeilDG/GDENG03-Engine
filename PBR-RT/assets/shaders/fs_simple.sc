$input v_worldPos, v_normal, v_texcoord0, v_color0

#include <bgfx_shader.sh>
#include "include/pbr_common.sh"

// PBR uniforms
uniform vec4 u_lightDir;      // xyz: direction, w: intensity
uniform vec4 u_baseColor;     // rgba: albedo color
uniform vec4 u_pbrParams;     // x: metallic, y: roughness, z: ao, w: unused
uniform vec4 u_cameraPos;     // xyz: camera world position, w: unused
uniform vec4 u_enableIBL;     // x: 0.0 = disabled, 1.0 = enabled
samplerCube u_irradianceMap;  // Precomputed irradiance for diffuse IBL
samplerCube u_prefilterMap;   // Prefiltered environment for specular IBL (with mips)
sampler2D u_brdfLUT;          // BRDF integration LUT (split-sum)

#define PI 3.14159265359

// Fresnel-Schlick with roughness for IBL
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3_splat(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // Extract PBR parameters
    float metallic = u_pbrParams.x;
    float roughness = u_pbrParams.y;
    float ao = u_pbrParams.z;

    roughness = max(roughness, 0.04);

    vec3 N = normalize(v_normal);
    vec3 V = normalize(u_cameraPos.xyz - v_worldPos);
    vec3 L = normalize(-u_lightDir.xyz);
    vec3 H = normalize(V + L);

    vec3 F0 = vec3_splat(0.04);
    F0 = mix(F0, u_baseColor.rgb, metallic);

    vec3 specular = BRDF_Specular_GGX(N, V, L, F0, roughness);
    vec3 F = F_Schlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3_splat(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 diffuse = kD * u_baseColor.rgb / PI;

    float NdotL = max(dot(N, L), 0.0);
    float lightIntensity = u_lightDir.w;
    vec3 radiance = vec3_splat(lightIntensity);

    vec3 Lo = (diffuse + specular) * radiance * NdotL;

    vec3 ambient;
    if (u_enableIBL.x > 0.5) {
        vec3 irradiance = textureCube(u_irradianceMap, N).rgb;
        irradiance = pow(abs(irradiance), vec3_splat(2.2)); // toLinear
        vec3 diffuseIBL = irradiance * u_baseColor.rgb;

        vec3 R = reflect(-V, N);
        const float MAX_REFLECTION_LOD = 4.0; // 5 mip levels (0-4)
        float lod = roughness * MAX_REFLECTION_LOD;
        vec3 prefilteredColor = textureCubeLod(u_prefilterMap, R, lod).rgb;
        prefilteredColor = pow(abs(prefilteredColor), vec3_splat(2.2)); // toLinear

        float NoV = max(dot(N, V), 0.0);

        vec3 F_ibl = fresnelSchlickRoughness(NoV, F0, roughness);
        vec3 kS_ibl = F_ibl;
        vec3 kD_ibl = vec3_splat(1.0) - kS_ibl;
        kD_ibl *= 1.0 - metallic;

        float iblDiffuseStrength = 2.0;
        float iblSpecularStrength = 2.5;

        vec3 diffuse_ambient = kD_ibl * diffuseIBL * iblDiffuseStrength;
        vec2 brdf = texture2D(u_brdfLUT, vec2(NoV, roughness)).rg;
        vec3 specular_ambient = prefilteredColor * (F0 * brdf.x + brdf.y);
        specular_ambient *= iblSpecularStrength;

        ambient = (diffuse_ambient + specular_ambient) * ao;
    } else {
        vec3 kS_ambient = F_Schlick(max(dot(N, V), 0.0), F0);
        vec3 kD_ambient = vec3_splat(1.0) - kS_ambient;
        kD_ambient *= 1.0 - metallic;
        ambient = (kD_ambient * u_baseColor.rgb + kS_ambient * F0) * ao * 0.6;
    }

    vec3 color = Lo + ambient;

    color = max(vec3_splat(0.0), color - 0.004);
    color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);

    gl_FragColor = vec4(color, u_baseColor.a);
}
