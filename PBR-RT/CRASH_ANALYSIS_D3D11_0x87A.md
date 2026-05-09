# D3D11 Exception 0x87A Root Cause Analysis & Fix

## Exception Details
- **Exception Code**: `0x0000087A` (Windows HIDWORD exception)
- **Location**: `KernelBase.dll` (raised by D3D11 debug layer validation)
- **Triggered At**: `AnitoRenderer::generatePrefilterMap()` → `bgfx::Draw()` call in D3D11 backend
- **D3D11 Error**: `ID3D11DeviceContext::Draw: DEVICE_DRAW_VIEW_DIMENSION_MISMATCH`

## Root Cause: Shader Resource View Dimension Mismatch

### The Problem
The prefiltered environment map (cubemap) texture was being created with **incorrect layer count**, causing the D3D11 backend to construct a **Cubemap Array** instead of a regular **Cubemap with Mip Chain**.

### Technical Details

**In `AnitoRenderer.cpp::generatePrefilterMap()` (line ~615):**
```cpp
m_prefilterMap = bgfx::createTextureCube(
    prefilterSize,     // 512
    true,              // hasMips = enable mip chain
    numMips,           // ← BUG: 5 (interpreted as numLayers, not mipCount!)
    bgfx::TextureFormat::RGBA16F,
    BGFX_TEXTURE_RT | ...
);
```

**API Misunderstanding**: The third parameter to `bgfx::createTextureCube()` is **`numLayers`** (number of cubemap array slices), NOT the number of mip levels.

### Consequences

1. **Incorrect Texture Creation**:
   - Intent: Create a single cubemap with 5 mip levels for specular roughness sampling
   - Result: Created a cubemap array with 5 array slices

2. **D3D11 SRV Mismatch** (debugger data confirmed):
   ```
   Texture 6 (m_prefilterMap):
     m_numLayers = 5        ← Treated as array layer count
     m_numMips = 10         ← Auto-generated
     m_type = 2             ← Cubemap
     SRV ArraySize = 5      ← Creates TEXTURECUBEARRAY
   ```

3. **Shader Binding Error**:
   - Shader declares: `SAMPLERCUBE(s_prefilterMap, 5)` → expects `TEXTURECUBE` type
   - D3D11 creates: `TEXTURECUBEARRAY` type (because ArraySize = 5)
   - Result: Type mismatch → validation error

4. **D3D11 Debug Layer Output**:
   ```
   D3D11 ERROR: ID3D11DeviceContext::Draw: The Shader Resource View 
   dimension declared in the shader code (TEXTURECUBE) is not an array 
   type but the view bound to slot 5 of the Pixel Shader unit has 
   ArraySize 5. This mismatch is invalid if the shader actually uses 
   the view. [EXECUTION ERROR #354: DEVICE_DRAW_VIEW_DIMENSION_MISMATCH]
   ```

## The Fix

**File**: `src/Renderer/AnitoRenderer.cpp`
**Function**: `AnitoRenderer::generatePrefilterMap()`
**Line**: ~615

### Change
```cpp
// BEFORE (WRONG):
m_prefilterMap = bgfx::createTextureCube(
    prefilterSize,
    true,    // hasMips
    numMips, // ← Interpreted as numLayers!
    bgfx::TextureFormat::RGBA16F,
    ...
);

// AFTER (CORRECT):
m_prefilterMap = bgfx::createTextureCube(
    prefilterSize,
    true,    // hasMips - automatically generates mip chain
    1,       // numLayers - MUST be 1 for single cubemap (not array)
    bgfx::TextureFormat::RGBA16F,
    ...
);
```

### Why This Works

1. **Single Cubemap**: `numLayers = 1` creates one cubemap (not an array)
2. **Automatic Mip Generation**: `hasMips = true` auto-generates mip chain (5 levels)
3. **Correct SRV Type**: D3D11 creates `TEXTURECUBE` SRV (not `TEXTURECUBEARRAY`)
4. **Shader Compatibility**: `SAMPLERCUBE` sampler can bind to `TEXTURECUBE` SRV
5. **Mip Sampling**: `textureCubeLod(s_prefilterMap, R, lod)` in shader correctly samples mip levels

## Key Insights

1. **bgfx API Contract**: In `createTextureCube()`, the third parameter is **always** `numLayers`, regardless of mip settings. Mip levels are controlled separately.

2. **Mip Chain vs Array Slices**: Two different concepts:
   - **Mip chain**: Progressive reduction of a single image (512x512 → 256x256 → ... → 1x1)
   - **Array slices**: Separate independent images stored contiguously

3. **D3D11 Validation**: The debug layer correctly rejected the mismatch before it could cause GPU memory corruption or undefined behavior.

4. **Shader Side**: The deferred lighting shader in `fs_deferred_light.sc` correctly uses `textureCubeLod()` to sample prefilter map at different mip levels based on roughness.

## Testing Recommendations

After applying the fix:
1. Run the application and verify IBL system initializes without crashes
2. Confirm prefilter map displays correct roughness-based specular reflections
3. Verify D3D11 debug layer shows no errors during scene rendering
4. Check that rougher surfaces (high roughness) show blurrier reflections

## Related Code Areas

- **Shader code** (correct usage): `assets/shaders/fs_deferred_light.sc` (line ~128)
  ```glsl
  float lod = roughness * MAX_REFLECTION_LOD;
  vec3 prefilteredColor = textureCubeLod(s_prefilterMap, R, lod).rgb;
  ```

- **Unused constant** (hint): `src/Renderer/AnitoPBRTestScenes.cpp` (end of file)
  ```cpp
  constexpr uint16_t kPrefilterCubeLayers = 1;  // ← Should have been used!
  ```

## Exception Analysis Summary

| Aspect | Details |
|--------|---------|
| Exception Type | Windows/D3D11 Debug Layer Validation Error |
| Root Cause | API parameter confusion (numLayers vs mipCount) |
| Manifest | SRV dimension mismatch at draw time |
| Fix Complexity | Single-line change (5 → 1) |
| Impact | Critical - prevents all rendering with IBL |
| Regression Risk | Very low - change aligns with shader expectations |
