// G-Buffer Fragment Shader
// Deferred rendering - Geometry pass with Multiple Render Targets (MRT)
// Outputs material properties to 4 render targets for deferred lighting
// Reference: BGFX Example 21-deferred, Filament material encoding, Real-Time Rendering 4th Ed.

$input v_worldPos, v_normal, v_texcoord0

#include <bgfx_shader.sh>

// PBR Material uniforms
uniform vec4 u_baseColor;     // RGB: Albedo color, A: unused
uniform vec4 u_pbrParams;     // x: metallic, y: roughness, z: ao, w: unused

// Helper function: Encode normal from [-1, 1] to [0, 1] range for RGBA8 storage
// Standard octahedron encoding could be used for better precision, but simple encoding is sufficient
vec3 encodeNormal(vec3 normal) {
    return normal * 0.5 + 0.5;  // Map [-1,1] to [0,1]
}

void main()
{
    // Normalize interpolated normal (can become denormalized during rasterization)
    vec3 N = normalize(v_normal);

    // Extract PBR parameters from uniform
    vec3 albedo = u_baseColor.rgb;
    float metallic = u_pbrParams.x;
    float roughness = u_pbrParams.y;
    float ao = u_pbrParams.z;

    // Ensure roughness is clamped to avoid numerical issues in lighting pass
    // Minimum roughness of 0.04 prevents singularities in GGX BRDF
    roughness = max(roughness, 0.04);

    // MRT Output Layout (must match AnitoGBuffer texture order):
    // RT0 (RGBA8): Albedo (RGB) + Metallic (A)
    // RT1 (RGBA8): Normal (RGB, encoded [0,1]) + Roughness (A)
    // RT2 (RGBA16F): World Position (RGB) + Ambient Occlusion (A)
    // RT3 (RGBA8): Emission (RGB) + Unused (A)

    // RT0: Albedo + Metallic
    // Albedo is stored in sRGB space (GPU will convert on write if format is sRGB-aware)
    // Metallic is linear [0,1]
    gl_FragData[0] = vec4(albedo, metallic);

    // RT1: Normal + Roughness
    // Normal encoded to [0,1] for RGBA8 storage (will decode in lighting pass)
    // Roughness is linear [0,1]
    gl_FragData[1] = vec4(encodeNormal(N), roughness);

    // RT2: World Position + AO
    // Position requires float precision (RGBA16F format)
    // AO is linear [0,1]
    gl_FragData[2] = vec4(v_worldPos, ao);

    // RT3: Emission (currently black - no emissive materials yet)
    // Future: Support emissive materials for bloom, glow effects
    gl_FragData[3] = vec4(0.0, 0.0, 0.0, 1.0);
}
