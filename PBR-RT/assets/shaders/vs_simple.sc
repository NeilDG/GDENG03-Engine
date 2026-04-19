$input a_position, a_normal, a_texcoord0, a_color0
$output v_worldPos, v_normal, v_texcoord0, v_color0

#include <bgfx_shader.sh>

void main()
{
    // Transform position to world space using built-in u_model
    vec4 worldPos = mul(u_model[0], vec4(a_position, 1.0));
    v_worldPos = worldPos.xyz;

    // Transform normal to world space (using normal matrix from u_model)
    mat3 normalMatrix = mat3(u_model[0][0].xyz, u_model[0][1].xyz, u_model[0][2].xyz);
    v_normal = normalize(mul(normalMatrix, a_normal));

    // Pass through texture coordinates and color
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;

    // Transform to clip space using bgfx built-in matrices
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
