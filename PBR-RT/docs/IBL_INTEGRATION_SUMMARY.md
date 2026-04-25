# IBL and Skybox Integration - Implementation Summary

## Overview
This document summarizes the changes made to integrate Image-Based Lighting (IBL) and skybox rendering into the Anito Engine PBR system.

## Files Modified

### 1. **Shader Files**

#### `assets/shaders/fs_simple.sc` (PBR Fragment Shader)
- **Added:** `samplerCube u_envMap` uniform for environment cubemap
- **Added:** `uniform int u_enableIBL` to toggle IBL on/off
- **Modified:** Ambient lighting section to support IBL
  - When IBL is enabled: Samples environment map for both diffuse and specular contributions
  - When IBL is disabled: Uses the old simplified ambient lighting
- **IBL Implementation:**
  - Diffuse: Samples cubemap using surface normal
  - Specular: Samples cubemap using reflection vector
  - Proper Fresnel and energy conservation applied

#### `assets/shaders/fs_skybox.sc` (NEW)
- Fragment shader for rendering the skybox
- Samples the environment cubemap
- Includes atmospheric effects (horizon fade, gamma correction)
- Reconstructs view direction from screen position

#### `assets/shaders/vs_skybox.sc` (NEW)
- Vertex shader for skybox rendering
- Passes through position for fragment shader
- Outputs full-screen triangle/quad

#### `assets/shaders/varying_skybox.def.sc` (NEW)
- Defines the varying `v_position` for skybox shaders

### 2. **Renderer (AnitoRenderer)**

#### `src/Renderer/AnitoRenderer.h`
- **Added Methods:**
  - `void loadEnvironmentMap(const std::string& hdrPath)` - Load HDR and create cubemap
  - `void renderSkybox(const float* viewProjInv, const float* cameraPos)` - Render skybox pass
  - `bgfx::TextureHandle getEnvCubemap() const` - Get cubemap handle for materials
  - `bool isIBLReady() const` - Check if IBL is available
  - `void setEnableIBL(bool enable)` - Toggle IBL
  - `bool getEnableIBL() const` - Get IBL state

- **Added Member Variables:**
  - `bgfx::TextureHandle m_envCubemap` - Environment cubemap texture
  - `bgfx::ProgramHandle m_skyboxProgram` - Skybox shader program
  - `bgfx::UniformHandle m_skyboxCubeUniform` - Skybox sampler uniform
  - `bool m_enableIBL` - IBL toggle state (default: true)

#### `src/Renderer/AnitoRenderer.cpp`
- **Implemented:** `loadEnvironmentMap()`
  - Creates a 1024x1024 placeholder cubemap (RGBA16F format)
  - Loads skybox shaders if available
  - Creates cubemap sampler uniform
  - **Note:** Full HDR loading requires stb_image integration (marked as TODO)

- **Implemented:** `renderSkybox()`
  - Placeholder for full skybox rendering
  - Will set uniforms and submit draw calls when integrated with scene

### 3. **Input Manager (AnitoInputManager)**

#### `src/Input/AnitoInputManager.h`
- **Added:** Forward declarations for `AnitoRenderer` and `AnitoPBRTestScenes`
- **Added Member Variables:**
  - `bool m_wasZPressed` - Track Z key state for IBL toggle
  - `bool m_wasSpacePressed` - Track space key state for scene switching

#### `src/Input/AnitoInputManager.cpp`
- **Modified:** `update()` method to handle:
  - **Z Key:** Toggle IBL on/off via `AnitoRenderer::setEnableIBL()`
  - **Space Key:** Switch to next test scene via `AnitoPBRTestScenes::switchToNextScene()`
- **Added:** Console output for IBL toggle and scene switching

### 4. **Test Scenes (AnitoPBRTestScenes)**

#### `src/Renderer/AnitoPBRTestScenes.h`
- **Added:** Static singleton accessor
  - `static AnitoPBRTestScenes* s_currentInstance`
  - `static AnitoPBRTestScenes* getCurrentInstance()`
- This allows the input manager to access the test scenes for scene switching

#### `src/Renderer/AnitoPBRTestScenes.cpp`
- **Modified:** Constructor to set `s_currentInstance = this`
- **Modified:** Destructor to clear `s_currentInstance` if it matches
- This ensures the singleton pattern works correctly

## Key Features Implemented

### 1. **IBL Toggle (Z Key)**
- Press Z to enable/disable Image-Based Lighting
- When enabled: Materials use environment map for ambient/specular
- When disabled: Materials use simplified ambient lighting
- Console feedback shows current IBL state

### 2. **Scene Switching (Space Key)**
- Press Space to cycle through test scenes:
  - Sphere Grid (6x6): X-axis = roughness, Y-axis = metallic
  - Cube Grid (6x6): Random rotations with varying PBR params
  - Mixed Materials: Alternating spheres and cubes
  - Metallic Showcase: Common metals
  - Dielectric Showcase: Non-metallic materials

### 3. **Environment Map System**
- Placeholder cubemap created (1024x1024, RGBA16F)
- Skybox shaders prepared for rendering
- IBL sampling in PBR shader ready to use

## Integration Status

### ✅ Completed
- [x] PBR shader updated with IBL support
- [x] Skybox shaders created
- [x] Renderer infrastructure for cubemap/skybox
- [x] Input handling for IBL toggle and scene switching
- [x] Test scenes singleton accessor
- [x] Build successful

### ⏳ TODO (Future Work)
- [ ] Full HDR loading with stb_image
- [ ] Equirectangular to cubemap conversion shader
- [ ] Skybox rendering pass in main loop
- [ ] Pass cubemap to materials in test scenes
- [ ] Irradiance and prefiltered environment map generation
- [ ] BRDF LUT for split-sum approximation

## How to Use

### In Your Main Render Loop:
```cpp
// Initialize renderer
AnitoRenderer::initialize(windowHandle, width, height);
auto* renderer = AnitoRenderer::getInstance();

// Load environment map
renderer->loadEnvironmentMap("assets/hdr/your_environment.hdr");

// Create test scenes
AnitoPBRTestScenes testScenes;
testScenes.initialize(pbrShader);

// In update loop:
inputManager->update();  // Handles Z for IBL, Space for scene switching
testScenes.update(deltaTime);  // Rotate cubes

// In render loop:
renderer->beginFrame();
// ... render test scene objects with PBR materials ...
// renderer->renderSkybox(viewProjInv, cameraPos);  // When implemented
renderer->endFrame();
```

### Passing Cubemap to Materials:
```cpp
// When creating materials, set the cubemap and IBL flag
material->setCubemap(renderer->getEnvCubemap());
material->setIBLEnabled(renderer->getEnableIBL());
```

## Testing Checklist

- [ ] Press Z to toggle IBL - console shows "IBL enabled/disabled"
- [ ] Press Space to switch scenes - console shows scene name
- [ ] 6x6 sphere grid displays with roughness/metallic variation
- [ ] 6x6 cube grid rotates with random axes
- [ ] PBR materials respond correctly to IBL toggle
- [ ] Environment reflections visible on metallic surfaces (when HDR loaded)
- [ ] Skybox renders as background (when implemented)

## Notes

- **stb_image:** Not included in current workspace. Add it to load actual HDR files.
- **Shader Compilation:** Run shader compiler to generate `.bin` files for skybox shaders.
- **Cubemap Conversion:** Full equirectangular-to-cubemap conversion requires compute shader or offscreen rendering.
- **Material Integration:** Ensure `AnitoMaterial` has methods to set cubemap and IBL flag.

## References

- Filament PBR Documentation: https://google.github.io/filament/Filament.html
- PolyHaven HDR Maps: https://polyhaven.com/hdris
- bgfx Cubemap API: https://bkaradzic.github.io/bgfx/index.html

---

**Implementation Date:** 2025
**Build Status:** ✅ Successful
**Next Steps:** Integrate stb_image, implement cubemap conversion, and connect to main render loop
