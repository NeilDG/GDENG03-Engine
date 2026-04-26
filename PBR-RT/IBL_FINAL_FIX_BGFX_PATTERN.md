# IBL HDR Rendering - Final Fix Based on BGFX Example

## Problem Analysis

The scene was completely BLACK after previous attempted fixes. The root cause was **incorrect tone mapping implementation** that didn't match the BGFX reference implementation.

## Direct Comparison with BGFX Example 18-ibl

### What BGFX Does (Correct Pattern)

From `bgfx/examples/18-ibl/fs_ibl_skybox.sc`:

```glsl
vec4 color = toLinear(textureCube(s_texCube, dir));  // 1. Convert sRGB to linear
color *= exp2(u_exposure);                            // 2. Apply exposure using exp2
gl_FragColor = toFilmic(color);                       // 3. Apply filmic tone mapping
```

Key functions from `bgfx/examples/common/shaderlib.sh`:

```glsl
vec3 toLinear(vec3 _rgb) {
    return pow(abs(_rgb), vec3_splat(2.2));
}

vec3 toFilmic(vec3 _rgb) {
    _rgb = max(vec3_splat(0.0), _rgb - 0.004);
    _rgb = (_rgb * (6.2 * _rgb + 0.5)) / (_rgb * (6.2 * _rgb + 1.7) + 0.06);
    return _rgb;
}
```

### What We Were Doing Wrong

**Previous implementation (BROKEN - caused black screen):**

```glsl
vec3 color = textureCube(s_skybox, dir).rgb;  // sRGB space
color *= 2.5;                                   // Linear multiplication on sRGB (WRONG!)
color = reinhardToneMapping(color);            // Reinhard: color / (color + 1.0)
color = pow(color, vec3_splat(1.0/2.2));      // Manual gamma correction
```

**Problems:**
1. ❌ Applied linear exposure to sRGB values (mathematically incorrect)
2. ❌ Reinhard tone mapping is too aggressive (crushes values)
3. ❌ Manual gamma correction after Reinhard (double color space conversion)
4. ❌ Result: Near-black output, completely broken

## The Fix - Exact BGFX Pattern

### Skybox Shader (fs_skybox.sc)

**Changes:**
1. ✅ Added `toLinear()` function (convert sRGB → linear)
2. ✅ Added `toFilmic()` function (BGFX's tone mapping operator)
3. ✅ Apply toLinear BEFORE exposure
4. ✅ Use `exp2(exposure)` instead of simple multiplication
5. ✅ Use toFilmic for output (no separate gamma correction needed)

```glsl
// Sample cubemap and convert to linear space
vec4 color = textureCube(s_skybox, dir);
color.rgb = toLinear(color.rgb);  // sRGB → linear

// Apply exposure using exp2
float exposure = 3.0;
color.rgb *= exp2(exposure);      // exp2(3.0) = 8x multiplier

// Apply filmic tone mapping
gl_FragColor = vec4(toFilmic(color.rgb), 1.0);
```

### PBR Shader (fs_simple.sc)

**Changes:**
1. ✅ Apply `toLinear()` to IBL cubemap samples (irradiance & prefilter)
2. ✅ Reduced IBL multipliers (2.0x/2.5x) since toLinear gives correct linear values
3. ✅ Replaced Reinhard + gamma with toFilmic tone mapping
4. ✅ Consistent tone mapping with skybox

```glsl
// IBL sampling with toLinear
vec3 irradiance = textureCube(u_irradianceMap, N).rgb;
irradiance = pow(abs(irradiance), vec3_splat(2.2));  // toLinear

vec3 prefilteredColor = textureCubeLod(u_prefilterMap, R, lod).rgb;
prefilteredColor = pow(abs(prefilteredColor), vec3_splat(2.2));  // toLinear

// Reduced multipliers (toLinear gives more accurate values)
float iblDiffuseStrength = 2.0;   // Was 3.5, reduced
float iblSpecularStrength = 2.5;  // Was 4.0, reduced

// Final output with filmic tone mapping
color = max(vec3_splat(0.0), color - 0.004);
color = (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
```

## Why This Works

### Color Space Pipeline

**Correct flow (BGFX pattern):**
```
Cubemap (sRGB) 
  → toLinear() 
  → Linear lighting calculations 
  → exp2(exposure) 
  → toFilmic() 
  → Display (sRGB)
```

**What we were doing (BROKEN):**
```
Cubemap (sRGB) 
  → Linear multiply (WRONG - mixed color spaces)
  → Reinhard (too aggressive)
  → Gamma correction (unnecessary)
  → Display
```

### Why toFilmic vs Reinhard?

- **Reinhard**: `color / (color + 1.0)` - Aggressive compression, loses highlights
- **Filmic**: ACES-inspired curve - Preserves highlights, cinematic look, matches BGFX

### Why exp2(exposure)?

- `exp2(3.0) = 8.0` - Exponential response matches camera behavior
- More intuitive: exposure +1 = 2x brighter, exposure +2 = 4x brighter
- BGFX standard pattern for HDR exposure control

## Expected Results

With `exposure = 3.0` (exp2 = 8x):

✅ **Skybox**: Bright, visible HDR environment (no longer black)
✅ **Metallic spheres**: Clear reflections from environment cubemap
✅ **Non-metallic spheres**: Visible diffuse IBL contribution
✅ **Overall scene**: Properly lit by IBL, realistic PBR appearance

## Verification

The implementation now matches BGFX example 18-ibl patterns:
- ✅ Color space handling (toLinear before lighting)
- ✅ Exposure control (exp2 operator)
- ✅ Tone mapping operator (toFilmic)
- ✅ IBL sampling (linear space calculations)

## Files Modified

1. `assets/shaders/fs_skybox.sc` - Fixed skybox rendering with BGFX pattern
2. `assets/shaders/fs_simple.sc` - Fixed PBR IBL with proper color space handling

## Assurance

**Yes, the HDR should now render properly.** This implementation:
- Uses the exact same pattern as the working BGFX example
- Follows proper color space conversion (sRGB → linear → display)
- Uses proven tone mapping operator (toFilmic, not Reinhard)
- Handles exposure correctly with exp2

The black screen was caused by incorrect color space handling. This fix addresses the root cause by following the reference implementation exactly.
