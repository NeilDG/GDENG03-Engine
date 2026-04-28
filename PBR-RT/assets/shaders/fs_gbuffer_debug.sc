$input v_texcoord0

#include <bgfx_shader.sh>

// G-Buffer textures
SAMPLER2D(s_gbuffer0, 0);  // Albedo + Metallic
SAMPLER2D(s_gbuffer1, 1);  // Normal + Roughness
SAMPLER2D(s_gbuffer2, 2);  // Position + AO
SAMPLER2D(s_gbuffer3, 3);  // Emission

void main()
{
    // Determine which quadrant we're rendering in (2x3 grid)
    vec2 uv = v_texcoord0;

    // Split screen into 2x3 grid (6 views total)
    // Top row: Albedo, Metallic, Normal
    // Bottom row: Roughness, Position, Emission

    float col = floor(uv.x * 3.0);  // 0, 1, or 2
    float row = floor(uv.y * 2.0);  // 0 or 1

    // Adjust UV to tile
    vec2 tileUV = vec2(
        fract(uv.x * 3.0),
        fract(uv.y * 2.0)
    );

    // Sample G-Buffer textures
    vec4 albedoMetallic = texture2D(s_gbuffer0, tileUV);
    vec4 normalRoughness = texture2D(s_gbuffer1, tileUV);
    vec4 positionAO = texture2D(s_gbuffer2, tileUV);
    vec4 emission = texture2D(s_gbuffer3, tileUV);

    // Decode normal from [0,1] to [-1,1] for visualization
    vec3 normal = normalRoughness.rgb * 2.0 - 1.0;
    normal = normalize(normal);
    // Re-encode to [0,1] for display
    normal = normal * 0.5 + 0.5;

    vec3 outputColor = vec3(0.0, 0.0, 0.0);

    // Top row (row == 0)
    if (row < 0.5) {
        if (col < 0.5) {
            // Top-left: Albedo (RGB)
            outputColor = albedoMetallic.rgb;
        } else if (col < 1.5) {
            // Top-middle: Metallic (grayscale)
            outputColor = vec3(albedoMetallic.a, albedoMetallic.a, albedoMetallic.a);
        } else {
            // Top-right: Normal (color-coded)
            outputColor = normal;
        }
    }
    // Bottom row (row == 1)
    else {
        if (col < 0.5) {
            // Bottom-left: Roughness (grayscale)
            outputColor = vec3(normalRoughness.a, normalRoughness.a, normalRoughness.a);
        } else if (col < 1.5) {
            // Bottom-middle: Position (normalized to 0-1 range for visualization)
            // Scale position from world space to visible range
            vec3 pos = positionAO.rgb;
            outputColor = abs(pos) * 0.1;  // Scale down for visibility
        } else {
            // Bottom-right: Emission
            outputColor = emission.rgb;
        }
    }

    gl_FragColor = vec4(outputColor, 1.0);
}
