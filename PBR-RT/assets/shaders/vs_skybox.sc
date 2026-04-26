$input a_position
$output v_dir

#include <bgfx_shader.sh>

uniform mat4 u_viewProjInv;
uniform vec4 u_cameraPos;

void main()
{
    // Output position (full screen triangle covering NDC space)
    gl_Position = vec4(a_position.xy, 1.0, 1.0);

    // Transform NDC position to world space direction
    // This approach matches BGFX's inverse projection method
    vec4 ndc = vec4(a_position.xy, 1.0, 1.0);
    vec4 worldPos = mul(u_viewProjInv, ndc);
    vec3 worldPosNormalized = worldPos.xyz / worldPos.w;

    // Direction from camera to world position
    v_dir = worldPosNormalized - u_cameraPos.xyz;
}
