// BRDF LUT Vertex Shader
// Fullscreen triangle pass for split-sum BRDF integration

$input a_position
$output v_texcoord0

#include <bgfx_shader.sh>

void main()
{
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_texcoord0 = a_position * 0.5 + 0.5;
}
