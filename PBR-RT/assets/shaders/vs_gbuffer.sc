// G-Buffer Vertex Shader
// Deferred rendering - Geometry pass
// Transforms geometry and passes data to fragment shader for MRT output
// Reference: BGFX Example 21-deferred, Real-Time Rendering 4th Ed. Ch. 20

$input a_position, a_normal, a_texcoord0
$output v_worldPos, v_normal, v_texcoord0

#include <bgfx_shader.sh>

void main()
{
    // Transform position to world space using u_model matrix
    // u_model[0] is the model matrix (bgfx built-in)
    vec4 worldPos = mul(u_model[0], vec4(a_position, 1.0));
    v_worldPos = worldPos.xyz;

    // Transform normal to world space
    // Using 3x3 upper-left of model matrix (assumes uniform scaling)
    // For non-uniform scaling, should use inverse-transpose, but that's expensive
    mat3 normalMatrix = mat3(
        u_model[0][0].xyz,
        u_model[0][1].xyz,
        u_model[0][2].xyz
    );
    v_normal = normalize(mul(normalMatrix, a_normal));

    // Pass through texture coordinates (no transformation)
    v_texcoord0 = a_texcoord0;

    // Transform to clip space for rasterization
    // u_modelViewProj is pre-computed by bgfx (Model * View * Projection)
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
