# Anito Engine - Image-Based Lighting (IBL) Guide

**Physically-Based Rendering with HDR Environments**  
**DLSU GAME Lab - Project Anito**

---

## Table of Contents
1. [Overview](#overview)
2. [Theory](#theory)
3. [Implementation](#implementation)
4. [Usage](#usage)
5. [Testing & Verification](#testing--verification)
6. [Troubleshooting](#troubleshooting)

---

## Overview

Image-Based Lighting (IBL) is a rendering technique that uses HDR environment maps to provide realistic ambient lighting and reflections for PBR materials.

### What is IBL?

IBL captures real-world lighting from HDR photographs (360° panoramas) and uses them to:
- **Diffuse lighting** - Indirect illumination from all directions
- **Specular reflections** - Glossy and mirror-like surface reflections
- **Skybox** - Visible environment background

### Benefits

✅ **Realistic lighting** - Captures real-world complexity  
✅ **Ambient occlusion** - Soft shadows in crevices  
✅ **Reflections** - Metallic surfaces reflect environment  
✅ **Performance** - Pre-computed, no runtime cost  
✅ **Artist-friendly** - Swap HDR images to change mood

---

## Theory

### PBR + IBL Equation

The full PBR lighting equation with IBL:

```
L_out = L_direct + L_indirect
```

Where:
- **L_direct** = Directional/point/spot lights (dynamic)
- **L_indirect** = IBL (pre-computed from environment)

### IBL Components

```
┌─────────────────────────────────────────────────────────────┐
│                  HDR Environment (.hdr)                      │
│              (Equirectangular panorama)                      │
└──────────────────────┬──────────────────────────────────────┘
                       │
          ┌────────────┴────────────┐
          │                         │
          ▼                         ▼
┌──────────────────┐     ┌──────────────────┐
│  Cubemap Convert │     │   Skybox         │
│  (6 faces)       │     │   Rendering      │
└────────┬─────────┘     └──────────────────┘
         │
    ┌────┴────┐
    │         │
    ▼         ▼
┌─────────┐ ┌──────────────┐
│Irradiance│ │Prefilter Map │
│   Map    │ │(+ BRDF LUT)  │
│ (Diffuse)│ │ (Specular)   │
└─────────┘ └──────────────┘
```

#### 1. Irradiance Map (Diffuse IBL)

- **Purpose:** Diffuse ambient lighting
- **Process:** Convolve environment with cosine-weighted hemisphere
- **Size:** Small (32x32 or 64x64 per face)
- **Usage:** Multiply with albedo for diffuse contribution

#### 2. Prefilter Map (Specular IBL)

- **Purpose:** Glossy/mirror reflections
- **Process:** Convolve environment with GGX distribution at different roughness levels
- **Size:** Mipmapped cubemap (e.g., 512x512, 256x256, 128x128, ..., 1x1)
- **Usage:** Sample based on roughness (rougher = lower mip)

#### 3. BRDF Lookup Texture (Split-Sum Approximation)

- **Purpose:** Pre-integrated BRDF term
- **Process:** Pre-compute BRDF integral for NdotV and roughness
- **Size:** 2D texture (512x512)
- **Usage:** Lookup table for specular IBL

### The Split-Sum Approximation

Epic Games' optimization for real-time IBL:

```glsl
// Specular IBL = Pre-filtered environment * BRDF
vec3 specularIBL = textureCubeLod(s_prefilterMap, R, roughness * maxMipLevel).rgb;
vec2 brdf = texture2D(s_brdfLUT, vec2(NdotV, roughness)).rg;
vec3 specular = specularIBL * (F * brdf.x + brdf.y);
```

---

## Implementation

### File Structure

```
src/Renderer/
├── AnitoRenderer.h/cpp           # Main renderer with IBL support
├── AnitoTexture.h/cpp            # Texture loading (HDR, cubemaps)
└── AnitoShader.h/cpp             # Shader management

assets/
└── shaders/
    ├── vs_skybox.sc              # Skybox vertex shader
    ├── fs_skybox.sc              # Skybox fragment shader (tone mapping)
    ├── vs_pbr.sc                 # PBR vertex shader
    └── fs_pbr.sc                 # PBR fragment shader (IBL integration)

assets/environments/
└── *.hdr                         # HDR environment maps
```

### Step 1: Load HDR Environment

```cpp
// In AnitoRenderer.cpp
void AnitoRenderer::loadHDREnvironment(const std::string& path) {
    // Load HDR image (equirectangular)
    AnitoTexture* hdriTexture = AnitoTexture::loadHDR(path);

    // Convert to cubemap
    m_environmentCubemap = convertEquirectangularToCubemap(hdriTexture);

    // Generate IBL maps
    generateIBLMaps();

    // Cleanup HDRI texture (no longer needed)
    delete hdriTexture;
}
```

### Step 2: Convert Equirectangular to Cubemap

```cpp
AnitoTexture* AnitoRenderer::convertEquirectangularToCubemap(AnitoTexture* hdri) {
    const uint32_t cubemapSize = 1024;

    // Create cubemap texture
    AnitoTexture* cubemap = AnitoTexture::createCubemap(
        cubemapSize,
        AnitoTexture::Format::RGBA16F,
        6  // 6 faces
    );

    // Render equirectangular to cubemap faces
    for (int face = 0; face < 6; ++face) {
        bgfx::setViewFrameBuffer(view, cubemap->getHandle(), face);

        // Set projection for this face
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureView = getCubemapViewMatrix(face);

        bgfx::setViewTransform(view, glm::value_ptr(captureView), glm::value_ptr(captureProjection));

        // Render cube with equirect→cubemap shader
        renderCube(equirectToCubemapShader, hdri);
    }

    return cubemap;
}
```

### Step 3: Generate Irradiance Map

```cpp
AnitoTexture* AnitoRenderer::generateIrradianceMap(AnitoTexture* environmentCubemap) {
    const uint32_t irradianceSize = 32; // Small is enough

    // Create irradiance cubemap
    AnitoTexture* irradianceMap = AnitoTexture::createCubemap(
        irradianceSize,
        AnitoTexture::Format::RGBA16F,
        1  // No mipmaps
    );

    // Convolve each face
    for (int face = 0; face < 6; ++face) {
        bgfx::setViewFrameBuffer(view, irradianceMap->getHandle(), face);

        // Set projection/view for this face
        setupCubemapFaceView(face);

        // Render with irradiance convolution shader
        // (Samples environment in cosine-weighted hemisphere)
        renderCube(irradianceShader, environmentCubemap);
    }

    return irradianceMap;
}
```

### Step 4: Generate Prefilter Map

```cpp
AnitoTexture* AnitoRenderer::generatePrefilterMap(AnitoTexture* environmentCubemap) {
    const uint32_t prefilterSize = 512;
    const uint32_t maxMipLevels = 5; // 512, 256, 128, 64, 32

    // Create prefilter cubemap with mipmaps
    AnitoTexture* prefilterMap = AnitoTexture::createCubemap(
        prefilterSize,
        AnitoTexture::Format::RGBA16F,
        maxMipLevels
    );

    // Convolve each mip level
    for (uint32_t mip = 0; mip < maxMipLevels; ++mip) {
        float roughness = (float)mip / (float)(maxMipLevels - 1);
        uint32_t mipSize = prefilterSize >> mip; // Divide by 2^mip

        for (int face = 0; face < 6; ++face) {
            bgfx::setViewFrameBuffer(view, prefilterMap->getHandle(), face, mip);

            // Set projection/view
            setupCubemapFaceView(face);

            // Pass roughness to shader
            prefilterShader->setUniform("u_roughness", roughness);

            // Render with GGX importance sampling
            renderCube(prefilterShader, environmentCubemap);
        }
    }

    return prefilterMap;
}
```

### Step 5: Generate BRDF LUT

```cpp
AnitoTexture* AnitoRenderer::generateBRDF_LUT() {
    const uint32_t lutSize = 512;

    // Create 2D texture
    AnitoTexture* brdfLUT = AnitoTexture::create2D(
        lutSize, lutSize,
        AnitoTexture::Format::RG16F
    );

    // Render to texture
    bgfx::setViewFrameBuffer(view, brdfLUT->getHandle());

    // Render fullscreen quad with BRDF integration shader
    renderFullscreenQuad(brdfLUTShader);

    return brdfLUT;
}
```

### Step 6: Use in PBR Shader

**Fragment Shader (`fs_pbr.sc`):**

```glsl
$input v_worldPos, v_normal, v_texcoord0, v_view

#include <bgfx_shader.sh>

// IBL textures
SAMPLERCUBE(s_irradianceMap, 0);
SAMPLERCUBE(s_prefilterMap, 1);
SAMPLER2D(s_brdfLUT, 2);

// PBR uniforms
uniform vec4 u_baseColor;
uniform vec4 u_pbrParams;  // metallic, roughness, ao, unused

void main()
{
    vec3 N = normalize(v_normal);
    vec3 V = normalize(v_view);
    vec3 R = reflect(-V, N);

    float NdotV = max(dot(N, V), 0.0);

    // Material properties
    vec3 albedo = u_baseColor.rgb;
    float metallic = u_pbrParams.x;
    float roughness = u_pbrParams.y;
    float ao = u_pbrParams.z;

    // Fresnel (Schlick approximation)
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = F0 + (1.0 - F0) * pow(1.0 - NdotV, 5.0);

    // Diffuse IBL
    vec3 irradiance = textureCube(s_irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    // Specular IBL
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureCubeLod(s_prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture2D(s_brdfLUT, vec2(NdotV, roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    // Combine
    vec3 kS = F;  // Specular contribution
    vec3 kD = (1.0 - kS) * (1.0 - metallic);  // Diffuse contribution

    vec3 ambient = (kD * diffuse + specular) * ao;

    // Final color (ambient + direct lighting if any)
    vec3 color = ambient;

    gl_FragColor = vec4(color, 1.0);
}
```

### Step 7: Render Skybox

**Skybox Shader (`fs_skybox.sc`):**

```glsl
$input v_dir

#include <bgfx_shader.sh>
#include "../common/shaderlib.sh"  // For toLinear, toFilmic

SAMPLERCUBE(s_skybox, 0);
uniform vec4 u_exposure;  // x = exposure value

void main()
{
    vec3 dir = normalize(v_dir);

    // Sample environment (sRGB)
    vec3 color = textureCube(s_skybox, dir).rgb;

    // Convert to linear
    color = toLinear(color);

    // Apply exposure
    color *= exp2(u_exposure.x);

    // Tone mapping (Filmic)
    color = toFilmic(color);

    gl_FragColor = vec4(color, 1.0);
}
```

**Helper functions (`shaderlib.sh`):**

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

---

## Usage

### Loading an Environment

```cpp
// In your initialization code
renderer->loadHDREnvironment("assets/environments/studio_small.hdr");
```

### Setting PBR Material

```cpp
// Create material
auto material = std::make_shared<AnitoMaterial>();
material->setShader(pbrShader);

// Set PBR properties
material->setBaseColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));  // Red
material->setMetallic(1.0f);   // Fully metallic
material->setRoughness(0.2f);  // Smooth
material->setAO(1.0f);         // No occlusion

// Bind IBL textures
material->setTexture("s_irradianceMap", renderer->getIrradianceMap(), 0);
material->setTexture("s_prefilterMap", renderer->getPrefilterMap(), 1);
material->setTexture("s_brdfLUT", renderer->getBRDF_LUT(), 2);
```

### Runtime Toggle

```cpp
// Toggle IBL on/off for debugging
if (input->isKeyPressed(GLFW_KEY_Z)) {
    renderer->setIBLEnabled(!renderer->isIBLEnabled());
}
```

---

## Testing & Verification

### Visual Checks

#### ✅ Skybox
- **Expected:** Bright HDR environment visible in background
- **Common issue:** Dark/blue gradient → Skybox not rendering or tone mapping broken

#### ✅ Metallic Spheres (metallic = 1.0)
- **Smooth (roughness = 0.0):** Sharp, mirror-like reflections of environment
- **Rough (roughness = 1.0):** Blurred reflections
- **Common issue:** Black spheres → IBL textures not bound or prefilter map missing

#### ✅ Dielectric Spheres (metallic = 0.0)
- **Expected:** Bright, evenly lit diffuse surfaces (no reflections)
- **Common issue:** Too dark → Irradiance map not working

### Test Scene

```cpp
void createIBLTestScene() {
    // Load environment
    renderer->loadHDREnvironment("assets/environments/studio_small.hdr");

    // Create 5x5 grid of spheres
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
            float roughness = (float)x / 4.0f;
            float metallic = (float)y / 4.0f;

            auto sphere = createSphere(glm::vec3(x * 2.5f, y * 2.5f, 0.0f));
            sphere->material->setMetallic(metallic);
            sphere->material->setRoughness(roughness);
            sphere->material->setBaseColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        }
    }
}
```

**Expected result:**
- **Top row (metallic=1.0):** Mirror-like reflections, smooth to rough (left to right)
- **Bottom row (metallic=0.0):** Diffuse-only, no reflections
- **Middle rows:** Gradual transition

### Interactive Testing

```cpp
// Toggle IBL
if (input->isKeyPressed(GLFW_KEY_Z)) {
    iblEnabled = !iblEnabled;
    std::cout << "IBL: " << (iblEnabled ? "ON" : "OFF") << std::endl;
}

// Cycle through scenes
if (input->isKeyPressed(GLFW_KEY_SPACE)) {
    currentScene = (currentScene + 1) % numScenes;
    loadScene(currentScene);
}

// Adjust exposure
if (input->isKeyDown(GLFW_KEY_UP)) {
    exposure += 0.1f;
}
if (input->isKeyDown(GLFW_KEY_DOWN)) {
    exposure -= 0.1f;
}
```

---

## Troubleshooting

### Issue: Skybox is black or dark blue

**Possible causes:**
1. HDR not loaded correctly
2. Tone mapping broken
3. Cubemap conversion failed

**Solutions:**
1. Check HDR file path
2. Verify `toLinear()` and `toFilmic()` functions
3. Check cubemap texture creation

**Debug:**
```cpp
// Log HDR loading
ANITO_LOG_INFO("Renderer", "Loading HDR: " << path);
if (!hdriTexture) {
    ANITO_LOG_ERROR("Renderer", "Failed to load HDR");
}
```

---

### Issue: Spheres are black (no lighting)

**Possible causes:**
1. IBL textures not bound
2. Wrong texture slots
3. Shader uniforms not set

**Solutions:**
1. Verify texture binding:
   ```cpp
   material->setTexture("s_irradianceMap", irradianceMap, 0);
   material->setTexture("s_prefilterMap", prefilterMap, 1);
   material->setTexture("s_brdfLUT", brdfLUT, 2);
   ```

2. Check shader uniform names match

**Debug:**
```cpp
// Verify textures are valid
if (!irradianceMap || !irradianceMap->isValid()) {
    ANITO_LOG_ERROR("Renderer", "Irradiance map invalid");
}
```

---

### Issue: Metallic spheres don't reflect environment

**Possible causes:**
1. Prefilter map not generated
2. Wrong reflection vector
3. Roughness not passed to shader

**Solutions:**
1. Verify prefilter map generation
2. Check reflection calculation: `R = reflect(-V, N)`
3. Ensure roughness uniform is set

**Debug:**
```glsl
// In fragment shader
gl_FragColor = vec4(R, 1.0); // Should show reflection vector (colorful)
```

---

### Issue: Rough surfaces look wrong

**Possible cause:** Prefilter map mip levels not correct

**Solution:** Verify mip chain generation:
```cpp
const float MAX_REFLECTION_LOD = 4.0; // Must match prefilter mip count - 1
vec3 prefilteredColor = textureCubeLod(s_prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
```

---

### Issue: Environment too bright/dark

**Solution:** Adjust exposure:
```cpp
// In skybox shader
uniform vec4 u_exposure;  // x = exposure value (default: 0.0)
color *= exp2(u_exposure.x);
```

**Typical exposure values:**
- **Outdoor daylight:** -1.0 to 0.0
- **Indoor:** 0.0 to 1.0
- **Night:** 1.0 to 2.0

---

## Best Practices

1. ✅ **Use HDR environments** - Linear color space is critical for PBR
2. ✅ **Pre-compute IBL maps** - Don't regenerate every frame
3. ✅ **Cache BRDF LUT** - Generate once, reuse forever
4. ✅ **Use appropriate sizes:**
   - Environment cubemap: 1024x1024 or 2048x2048
   - Irradiance map: 32x32 (enough for diffuse)
   - Prefilter map: 512x512 with 5 mip levels
   - BRDF LUT: 512x512
5. ✅ **Test with known environments** - Compare with reference (Filament, Unreal)
6. ✅ **Profile performance** - IBL adds ~1-2ms per frame

---

## Reference

### HDR Environment Sources
- **Poly Haven:** https://polyhaven.com/hdris (Free CC0 HDRIs)
- **HDRI Haven:** https://hdrihaven.com/
- **sIBL Archive:** http://www.hdrlabs.com/sibl/archive.html

### Technical References
- **Epic Games' PBR notes:** https://blog.selfshadow.com/publications/s2013-shading-course/
- **Filament documentation:** https://google.github.io/filament/Filament.html
- **LearnOpenGL IBL:** https://learnopengl.com/PBR/IBL/Diffuse-irradiance

### Implementation Files
- `src/Renderer/AnitoRenderer.cpp` - IBL generation
- `assets/shaders/fs_pbr.sc` - PBR shader with IBL
- `assets/shaders/fs_skybox.sc` - Skybox rendering
