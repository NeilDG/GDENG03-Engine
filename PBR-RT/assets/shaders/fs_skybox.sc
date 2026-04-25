$input v_position
#include <bgfx_shader.sh>

uniform mat4 u_viewProjInv;
uniform vec4 u_cameraPos;
samplerCube s_skybox;

void main()
{
    // Reconstruct view direction from screen position
    vec2 uv = v_position.xy * 0.5 + 0.5;
    vec4 ndc = vec4(uv * 2.0 - 1.0, 1.0, 1.0);
    vec4 world = mul(u_viewProjInv, ndc);
    vec3 dir = normalize(world.xyz / world.w - u_cameraPos.xyz);

    // Sample cubemap
    vec3 color = textureCube(s_skybox, dir).rgb;

    // Simple atmospheric effect: fade horizon, gamma correction
    float horizon = pow(1.0 - abs(dir.y), 2.0);
    color = mix(color, vec3(0.6,0.7,1.0), horizon * 0.25);
    color = pow(color, vec3_splat(1.0/2.2));

    gl_FragColor = vec4(color, 1.0);
}
