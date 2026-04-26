$input v_dir
#include <bgfx_shader.sh>

samplerCube s_skybox;
uniform vec4 u_skyboxParams; // x: exposure

// Convert from sRGB to linear space
vec3 toLinear(vec3 _rgb)
{
    return pow(abs(_rgb), vec3_splat(2.2));
}

// Filmic tone mapping
vec3 toFilmic(vec3 _rgb)
{
    _rgb = max(vec3_splat(0.0), _rgb - 0.004);
    _rgb = (_rgb * (6.2 * _rgb + 0.5)) / (_rgb * (6.2 * _rgb + 1.7) + 0.06);
    return _rgb;
}

void main()
{
    // Normalize direction
    vec3 dir = normalize(v_dir);

    // Sample cubemap
    vec4 color = textureCube(s_skybox, dir);

    // Convert to linear space
    color.rgb = toLinear(color.rgb);

    // Apply exposure
    float exposure = u_skyboxParams.x;
    color.rgb *= exp2(exposure);

    // Apply tone mapping
    gl_FragColor = vec4(toFilmic(color.rgb), 1.0);
}
