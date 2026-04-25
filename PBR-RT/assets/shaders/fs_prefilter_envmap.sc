$input v_texcoord0

#include <bgfx_shader.sh>

samplerCube s_envMap;
uniform vec4 u_faceParams;    // x: face index, y: roughness, z: resolution
uniform vec4 u_sampleCount;   // x: sample count

#define PI 3.14159265359

// Van Der Corput sequence for quasi-random sampling
float radicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), radicalInverse_VdC(i));
}

// GGX importance sampling
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness * roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // Spherical to cartesian (tangent space)
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // Tangent space to world space
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

// Convert face index and UV to cubemap direction
vec3 getCubemapDirection(int face, vec2 uv) {
    vec3 dir;
    vec2 coord = uv * 2.0 - 1.0; // [-1, 1]

    if (face == 0) {        // +X
        dir = vec3(1.0, -coord.y, -coord.x);
    } else if (face == 1) { // -X
        dir = vec3(-1.0, -coord.y, coord.x);
    } else if (face == 2) { // +Y
        dir = vec3(coord.x, 1.0, coord.y);
    } else if (face == 3) { // -Y
        dir = vec3(coord.x, -1.0, -coord.y);
    } else if (face == 4) { // +Z
        dir = vec3(coord.x, -coord.y, 1.0);
    } else {                // -Z (face == 5)
        dir = vec3(-coord.x, -coord.y, -1.0);
    }

    return normalize(dir);
}

void main()
{
    int face = int(u_faceParams.x);
    float roughness = u_faceParams.y;
    vec3 N = getCubemapDirection(face, v_texcoord0);
    vec3 R = N; // For pre-filtering, R == V == N
    vec3 V = R;

    const uint SAMPLE_COUNT = uint(u_sampleCount.x);
    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3_splat(0.0);

    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        vec2 Xi = hammersley(i, SAMPLE_COUNT);
        vec3 H = importanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            prefilteredColor += textureCube(s_envMap, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / max(totalWeight, 0.001);

    gl_FragColor = vec4(prefilteredColor, 1.0);
}
