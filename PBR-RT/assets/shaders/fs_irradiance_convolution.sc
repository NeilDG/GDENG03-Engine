$input v_texcoord0

#include <bgfx_shader.sh>

samplerCube s_envMap;
uniform vec4 u_faceParams; // x: face index

#define PI 3.14159265359

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
    vec3 N = getCubemapDirection(face, v_texcoord0);

    // Tangent space from normal
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    // Convolve environment map using Riemann sum
    vec3 irradiance = vec3_splat(0.0);
    float sampleCount = 0.0;

    float sampleDelta = 0.025; // Sample step size

    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            // Spherical to Cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

            // Tangent space to world space
            vec3 sampleVec = tangentSample.x * tangent + tangentSample.y * bitangent + tangentSample.z * N;

            irradiance += textureCube(s_envMap, sampleVec).rgb * cos(theta) * sin(theta);
            sampleCount += 1.0;
        }
    }

    irradiance = PI * irradiance / sampleCount;

    gl_FragColor = vec4(irradiance, 1.0);
}
