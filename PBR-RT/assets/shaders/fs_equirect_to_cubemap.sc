$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_texEquirect, 0);
uniform vec4 u_faceParams; // (faceIndex, unused, unused, unused)

// Convert cubemap direction to equirectangular UV coordinates
vec2 dirToEquirect(vec3 dir)
{
    float phi = atan2(dir.z, dir.x);
    float theta = asin(dir.y);

    vec2 uv;
    uv.x = phi / (2.0 * 3.14159265359) + 0.5;
    uv.y = theta / 3.14159265359 + 0.5;

    return uv;
}

// Get direction for current cubemap face and texcoord
vec3 getDirection(int faceIndex, vec2 uv)
{
    vec3 dir;
    vec2 st = uv * 2.0 - 1.0; // Convert [0,1] to [-1,1]

    if (faceIndex == 0) {      // +X
        dir = vec3(1.0, -st.y, -st.x);
    } else if (faceIndex == 1) { // -X
        dir = vec3(-1.0, -st.y, st.x);
    } else if (faceIndex == 2) { // +Y
        dir = vec3(st.x, 1.0, st.y);
    } else if (faceIndex == 3) { // -Y
        dir = vec3(st.x, -1.0, -st.y);
    } else if (faceIndex == 4) { // +Z
        dir = vec3(st.x, -st.y, 1.0);
    } else {                     // -Z (faceIndex == 5)
        dir = vec3(-st.x, -st.y, -1.0);
    }

    return normalize(dir);
}

void main()
{
    int faceIndex = int(u_faceParams.x);
    vec3 dir = getDirection(faceIndex, v_texcoord0);
    vec2 equirectUV = dirToEquirect(dir);

    vec4 color = texture2D(s_texEquirect, equirectUV);

    gl_FragColor = color;
}
