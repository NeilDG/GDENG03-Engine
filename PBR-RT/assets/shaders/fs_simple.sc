$input v_worldPos, v_normal, v_texcoord0, v_color0

#include <bgfx_shader.sh>

// Simple PBR-style lighting uniforms
uniform vec4 u_lightDir;      // xyz: direction, w: intensity
uniform vec4 u_baseColor;     // rgba: albedo color
uniform vec4 u_pbrParams;     // x: metallic, y: roughness, z: ao, w: unused

void main()
{
    // Normalize interpolated normal
    vec3 normal = normalize(v_normal);

    // Simple directional light
    vec3 lightDir = normalize(u_lightDir.xyz);
    float ndotl = max(dot(normal, -lightDir), 0.0);

    // Simple lighting calculation (not full PBR yet, just for testing)
    vec3 ambient = u_baseColor.rgb * 0.6;  // Higher ambient for brighter base color
    vec3 diffuse = u_baseColor.rgb * ndotl * 1.2;  // Stronger direct lighting

    vec3 color = ambient + diffuse;

    // Apply vertex color if present (default to white if no vertex color)
    vec3 vertexColor = max(v_color0.rgb, vec3_splat(1.0));
    color *= vertexColor;

    // Ensure minimum brightness for visibility
    color = max(color, u_baseColor.rgb * 0.5);

    gl_FragColor = vec4(color, u_baseColor.a);
}
