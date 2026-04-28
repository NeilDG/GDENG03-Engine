// Deferred Lighting Vertex Shader
// Renders fullscreen triangle for deferred lighting pass
// Takes position input from fullscreen triangle/quad vertex buffer
// Reference: BGFX Example 21-deferred

$input a_position
$output v_texcoord0

#include <bgfx_shader.sh>

void main()
{
    // a_position contains NDC coordinates: xy in [-1, 1]
    // Direct passthrough to clip space
    gl_Position = vec4(a_position, 0.0, 1.0);

    // Generate UV coordinates from position
    // Map from [-1, 1] to [0, 1]
    v_texcoord0 = a_position * 0.5 + 0.5;

    // BGFX texture coordinate system: (0,0) is top-left
    // Let bgfx handle the coordinate system conversion
}
