# Anito Engine - Shader Pipeline

**Professional Shader Compilation & Management**  
**DLSU GAME Lab - Project Anito**

---

## Table of Contents
1. [Overview](#overview)
2. [Quick Setup](#quick-setup)
3. [Shader Compilation Workflow](#shader-compilation-workflow)
4. [Writing Shaders](#writing-shaders)
5. [Troubleshooting](#troubleshooting)
6. [Advanced Topics](#advanced-topics)

---

## Overview

Anito Engine uses **bgfx** as its rendering abstraction layer, which includes a sophisticated shader compilation system that:

- ✅ Cross-compiles GLSL-like shaders to multiple backends (Vulkan SPIR-V, DirectX HLSL, Metal)
- ✅ Handles platform-specific differences automatically
- ✅ Produces optimized binary shaders (.bin files)
- ✅ Integrates with CMake for automatic compilation

### Shader File Structure

```
assets/
└── shaders/
    ├── varying.def.sc          # Vertex attribute definitions (required)
    ├── vs_*.sc                 # Vertex shader sources
    ├── fs_*.sc                 # Fragment shader sources
    ├── cs_*.sc                 # Compute shader sources
    └── compiled/               # Binary outputs (.bin files)
        ├── vs_*.bin
        ├── fs_*.bin
        └── cs_*.bin
```

---

## Quick Setup

### Option 1: Automated Build Integration (Recommended)

Shaders are automatically compiled during the build process if `shaderc.exe` is available:

```powershell
# Build shaderc (one-time setup)
tools\build_shaderc.bat

# Rebuild project (shaders compile automatically)
Build.bat rebuild
```

**Done!** Shaders will now compile automatically on every build.

### Option 2: Manual Compilation

If you need to compile shaders without building the entire project:

```powershell
# Simple compilation script
compile_shaders_simple.bat

# Or using PowerShell
powershell -ExecutionPolicy Bypass -File tools\compile_shaders.ps1
```

### Option 3: Download Prebuilt Shaderc

If building shaderc fails:

1. Download bgfx tools: https://github.com/bkaradzic/bgfx/releases
2. Extract and find `shaderc.exe`
3. Copy to: `external/bgfx/tools/shaderc.exe`
4. Run: `compile_shaders_simple.bat`

---

## Shader Compilation Workflow

### How It Works

```
┌─────────────────┐
│  Source Shader  │  assets/shaders/vs_simple.sc
│   (.sc file)    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   shaderc.exe   │  bgfx shader compiler
│  (Cross-comp.)  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Binary Shader  │  assets/shaders/compiled/vs_simple.bin
│   (.bin file)   │  (Platform-specific: SPIR-V, DXBC, MSL)
└─────────────────┘
         │
         ▼
┌─────────────────┐
│  AnitoShader    │  Runtime loading
│   (Engine)      │
└─────────────────┘
```

### Compilation Targets

shaderc produces optimized shaders for:

| Platform | Backend | Shader Format | Profile |
|----------|---------|---------------|---------|
| **Windows** | Vulkan | SPIR-V | spirv |
| **Windows** | DirectX 11 | DXBC | s_5_0 (Shader Model 5.0) |
| **Windows** | DirectX 12 | DXBC | s_5_0 |
| **Android** | Vulkan | SPIR-V | spirv |
| **Linux** | Vulkan | SPIR-V | spirv |
| **macOS** | Metal | MSL | metal |

Anito Engine primarily targets:
- **Primary:** Vulkan (Windows, Android, Linux)
- **Secondary:** DirectX 11/12 (Windows fallback)

---

## Writing Shaders

### Basic Shader Structure

#### 1. Define Vertex Attributes (`varying.def.sc`)

This file **must exist** and defines data passed between shader stages:

```glsl
// assets/shaders/varying.def.sc

vec3 a_position  : POSITION;
vec3 a_normal    : NORMAL;
vec2 a_texcoord0 : TEXCOORD0;
vec4 a_color0    : COLOR0;

vec3 v_worldPos  : TEXCOORD1;
vec3 v_normal    : NORMAL;
vec2 v_texcoord0 : TEXCOORD0;
vec4 v_color0    : COLOR0;
vec3 v_view      : TEXCOORD2;
```

#### 2. Vertex Shader (`vs_pbr.sc`)

```glsl
$input a_position, a_normal, a_texcoord0, a_color0
$output v_worldPos, v_normal, v_texcoord0, v_color0, v_view

#include <bgfx_shader.sh>

uniform mat4 u_model;
uniform vec3 u_camPos;

void main()
{
    // Transform to world space
    vec4 worldPos = mul(u_model, vec4(a_position, 1.0));
    v_worldPos = worldPos.xyz;

    // Transform normal to world space
    mat3 normalMatrix = mat3(u_model);
    v_normal = normalize(mul(normalMatrix, a_normal));

    // Pass through texture coordinates and color
    v_texcoord0 = a_texcoord0;
    v_color0 = a_color0;

    // View direction
    v_view = normalize(u_camPos - worldPos.xyz);

    // Final position
    gl_Position = mul(u_viewProj, worldPos);
}
```

#### 3. Fragment Shader (`fs_pbr.sc`)

```glsl
$input v_worldPos, v_normal, v_texcoord0, v_color0, v_view

#include <bgfx_shader.sh>

// PBR material parameters
uniform vec4 u_baseColor;
uniform vec4 u_pbrParams;  // metallic, roughness, ao, unused

// IBL textures
SAMPLERCUBE(s_irradianceMap, 0);
SAMPLERCUBE(s_prefilterMap, 1);
SAMPLER2D(s_brdfLUT, 2);

void main()
{
    vec3 N = normalize(v_normal);
    vec3 V = normalize(v_view);

    // PBR lighting calculation
    vec3 albedo = u_baseColor.rgb * v_color0.rgb;
    float metallic = u_pbrParams.x;
    float roughness = u_pbrParams.y;
    float ao = u_pbrParams.z;

    // ... (PBR BRDF calculations)

    gl_FragColor = vec4(finalColor, 1.0);
}
```

### Shader Includes

bgfx provides common shader utilities in `external/bgfx/src/`:

```glsl
#include <bgfx_shader.sh>      // Core bgfx utilities, uniforms
#include <shaderlib.sh>         // Tone mapping, color space conversion
```

**Common utility functions:**
- `mul()` - Matrix/vector multiplication (cross-platform)
- `toLinear()` - sRGB to linear color space
- `toFilmic()` - Filmic tone mapping
- `encodeRgbE8()` / `decodeRgbE8()` - HDR encoding

---

## Troubleshooting

### Common Issues

#### ❌ "shaderc.exe not found"

**Solution:**
```powershell
# Option 1: Build from source
tools\build_shaderc.bat

# Option 2: Download prebuilt
# See "Option 3: Download Prebuilt Shaderc" above
```

#### ❌ Shader compilation errors

Check shader syntax:
```powershell
# Compile with verbose output
external\bgfx\tools\shaderc.exe -f assets/shaders/vs_simple.sc -o assets/shaders/compiled/vs_simple.bin --type vertex --platform windows -p spirv --verbose
```

Common errors:
- **Missing varying.def.sc**: All shaders need this file
- **Undefined uniforms**: Declare uniforms in shader source
- **Wrong input/output**: Check `$input` and `$output` match varying.def.sc
- **Platform-specific syntax**: Use bgfx's cross-platform functions (e.g., `mul()` instead of matrix * vector)

#### ❌ Black screen / No rendering

**Possible causes:**
1. Shaders not compiled (check `assets/shaders/compiled/` for .bin files)
2. Wrong shader path in code
3. Shader uniforms not set correctly
4. Vertex format mismatch

**Debug steps:**
```cpp
// In your code, verify shader loading
auto shader = AnitoShader::loadFromFile("vs_simple", "fs_simple");
if (!shader || !shader->isValid()) {
    std::cerr << "Shader load failed!" << std::endl;
}
```

#### ❌ Shader works on Windows but not Android

**Cause:** Platform-specific shader variations not compiled  
**Solution:** Compile for Android target:

```powershell
shaderc.exe -f vs_simple.sc -o vs_simple_spirv.bin --type vertex --platform android -p spirv
```

---

## Advanced Topics

### Automatic Compilation via CMake

CMakeLists.txt includes custom functions for shader compilation:

```cmake
function(compile_shader SHADER_FILE SHADER_TYPE OUTPUT_NAME)
    # Detect shaderc.exe location
    find_program(SHADERC_EXE 
        NAMES shaderc
        PATHS 
            "${CMAKE_SOURCE_DIR}/external/bgfx/tools"
            "${CMAKE_SOURCE_DIR}/external/bgfx/.build/win64_vs2022/bin"
    )

    # Add custom command
    add_custom_command(
        OUTPUT ${OUTPUT_FILE}
        COMMAND ${SHADERC_EXE} -f ${SHADER_FILE} -o ${OUTPUT_FILE} 
                --type ${SHADER_TYPE} --platform windows -p spirv
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling shader: ${SHADER_FILE}"
    )
endfunction()
```

### Multi-Platform Compilation

For production, compile shaders for all target platforms:

```powershell
# Windows - Vulkan (SPIR-V)
shaderc.exe -f vs_pbr.sc -o compiled/vs_pbr_spirv.bin --type vertex --platform windows -p spirv

# Windows - DirectX 11 (HLSL)
shaderc.exe -f vs_pbr.sc -o compiled/vs_pbr_dx11.bin --type vertex --platform windows -p s_5_0

# Android - Vulkan (SPIR-V)
shaderc.exe -f vs_pbr.sc -o compiled/vs_pbr_android.bin --type vertex --platform android -p spirv

# Linux - Vulkan (SPIR-V)
shaderc.exe -f vs_pbr.sc -o compiled/vs_pbr_linux.bin --type vertex --platform linux -p spirv
```

Runtime selection in `AnitoShader.cpp`:

```cpp
std::string AnitoShader::getPlatformShaderPath(const std::string& baseName) {
    bgfx::RendererType::Enum renderer = bgfx::getRendererType();

    switch(renderer) {
        case bgfx::RendererType::Vulkan:  return baseName + "_spirv.bin";
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12: return baseName + "_dx11.bin";
        case bgfx::RendererType::Metal:   return baseName + "_metal.bin";
        default:                          return baseName + "_spirv.bin";
    }
}
```

### Shader Hot-Reloading (Development Feature)

Enable shader hot-reloading for rapid iteration:

```cpp
// In AnitoRenderer.cpp
if (config->enableShaderHotReload) {
    shaderWatcher->watchDirectory("assets/shaders/");
    shaderWatcher->onFileChanged([](const std::string& file) {
        // Recompile and reload shader
        ShaderCompiler::compile(file);
        ShaderManager::reload(file);
    });
}
```

### Shader Variants & Permutations

For complex shaders with multiple features:

```glsl
// vs_pbr.sc
#if USE_NORMAL_MAP
    v_tangent = a_tangent;
    v_bitangent = a_bitangent;
#endif

#if USE_SKINNING
    // Skinning calculations
#endif
```

Compile variants:
```powershell
shaderc.exe -f vs_pbr.sc -o vs_pbr_base.bin --type vertex -p spirv
shaderc.exe -f vs_pbr.sc -o vs_pbr_normalmap.bin --type vertex -p spirv --define USE_NORMAL_MAP=1
shaderc.exe -f vs_pbr.sc -o vs_pbr_skinned.bin --type vertex -p spirv --define USE_SKINNING=1
```

### Debugging Shaders

**Visual Studio Graphics Debugger:**
1. Run engine with graphics debugging enabled
2. Capture frame
3. Inspect shader inputs/outputs

**RenderDoc Integration:**
```cpp
// Enable RenderDoc capture
#if defined(_DEBUG) && defined(USE_RENDERDOC)
    bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_STATS);
#endif
```

**Shader printf (Vulkan):**
```glsl
// In fragment shader
debugPrintfEXT("Normal: %f %f %f\n", v_normal.x, v_normal.y, v_normal.z);
```

---

## Integration with Engine

### Loading Shaders

```cpp
// In game code
auto pbrShader = AnitoShader::loadFromFile("vs_pbr", "fs_pbr");
material->setShader(pbrShader);
```

### Setting Uniforms

```cpp
// Set PBR parameters
shader->setUniform("u_baseColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
shader->setUniform("u_pbrParams", glm::vec4(metallic, roughness, ao, 0.0f));

// Bind textures
shader->setTexture("s_albedoMap", albedoTexture, 0);
shader->setTexture("s_normalMap", normalTexture, 1);
```

---

## Best Practices

1. ✅ **Keep shaders in version control**: Commit both .sc sources and compiled .bin files
2. ✅ **Use shader includes**: Reuse common functions via bgfx includes
3. ✅ **Document shader uniforms**: Comment expected uniform names and types
4. ✅ **Test on multiple backends**: Verify Vulkan and DirectX rendering
5. ✅ **Profile shader performance**: Use bgfx stats and GPU profilers
6. ✅ **Use shader variants**: Avoid uber-shaders with many branches
7. ✅ **Validate shader inputs**: Check vertex format matches shader expectations

---

## Reference

- **bgfx shader documentation**: https://bkaradzic.github.io/bgfx/tools.html#shader-compiler-shaderc
- **bgfx examples**: `external/bgfx/examples/` (18-ibl, 16-pbr, etc.)
- **Shader language guide**: bgfx uses GLSL-like syntax with cross-platform extensions

For shader-related issues, check:
1. Compilation logs in console output
2. `assets/shaders/compiled/` for generated .bin files
3. bgfx documentation for platform-specific quirks
