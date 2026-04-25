$input a_position
$output v_position

#include <bgfx_shader.sh>

void main()
{
    // Pass through position for fragment shader
    v_position = vec3(a_position.xy, 0.0);

    // Output position (full screen triangle/quad)
    gl_Position = vec4(a_position.xy, 1.0, 1.0);
}
