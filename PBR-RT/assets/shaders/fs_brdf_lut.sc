// BRDF LUT Fragment Shader
// Integrates GGX Cook-Torrance BRDF over hemisphere for split-sum IBL
// Output RG: scale and bias for specular IBL term

$input v_texcoord0

#include <bgfx_shader.sh>
#include "include/pbr_common.sh"

#define SAMPLE_COUNT 1024

float radicalInverseVdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint n)
{
    return vec2(float(i) / float(n), radicalInverseVdC(i));
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;

    float phi = 2.0 * ANITO_PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

vec2 integrateBRDF(float NoV, float roughness)
{
    vec3 V;
    V.x = sqrt(max(1.0 - NoV * NoV, 0.0));
    V.y = 0.0;
    V.z = NoV;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 Xi = hammersley(i, SAMPLE_COUNT);
        vec3 H = importanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NoL = max(L.z, 0.0);
        float NoH = max(H.z, 0.0);
        float VoH = max(dot(V, H), 0.0);

        if (NoL > 0.0)
        {
            float G = V_SmithGGXCorrelated(NoV, NoL, roughness);
            float G_Vis = (G * VoH) / max(NoH * NoV, 0.001);
            float Fc = pow(1.0 - VoH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    return vec2(A, B) / float(SAMPLE_COUNT);
}

void main()
{
    float NoV = clamp(v_texcoord0.x, 0.0, 1.0);
    float roughness = clamp(v_texcoord0.y, 0.04, 1.0);

    vec2 integratedBRDF = integrateBRDF(NoV, roughness);
    gl_FragColor = vec4(integratedBRDF, 0.0, 1.0);
}
