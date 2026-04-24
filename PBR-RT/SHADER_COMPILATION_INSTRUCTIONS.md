# Shader Compilation Setup - Anito Engine

## Summary of Changes

The shader compilation process has been simplified and automated to ensure shaders are always compiled during the build process.

## What Was Fixed

### 1. **CMakeLists.txt Updates**
   - Added `external/bgfx/tools/shaderc.exe` as the first path to check for the shader compiler
   - Fixed shader profile names from `vs_5_0`/`ps_5_0` to `s_5_0` (correct HLSL Shader Model 5.0 syntax)
   - Improved shader compilation function to properly set output dependencies
   - Created `CompileShaders` target that automatically builds shaders before the main executable

### 2. **Simplified Build Scripts**
   - **`compile_shaders_simple.bat`**: New simple script for manual shader compilation
   - **`compile_shaders.bat`**: Updated to check multiple shaderc locations
   - **`tools/compile_shaders.ps1`**: Updated to prioritize `external/bgfx/tools/shaderc.exe`

### 3. **Shader Improvements**
   - Increased ambient lighting from 0.3 to 0.6 for a brighter base color
   - Increased diffuse lighting from 0.7 to 1.2 for stronger illumination
   - Added minimum brightness clamp to ensure the cube is always visible (at least 50% of base color)

## How to Use

### Automatic Compilation (Recommended)
Shaders are now automatically compiled when you build the project:

```bash
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

The `CompileShaders` target is a dependency of `AnitoEngine`, so shaders will always be compiled first.

### Manual Compilation
If you need to compile shaders manually:

**Option 1: Simple Batch Script**
```bash
.\compile_shaders_simple.bat
```

**Option 2: Original Batch Script**
```bash
.\compile_shaders.bat
```

**Option 3: PowerShell Script**
```powershell
powershell -ExecutionPolicy Bypass -File tools/compile_shaders.ps1
```

**Option 4: CMake Target Only**
```bash
cmake --build out/build/x64-debug --target CompileShaders
```

## Shader Locations

### Source Shaders
- `assets/shaders/vs_simple.sc` - Vertex shader
- `assets/shaders/fs_simple.sc` - Fragment shader
- `assets/shaders/varying.def.sc` - Vertex attribute definitions

### Compiled Shaders
- `assets/shaders/compiled/vs_simple.bin` - Compiled vertex shader (DirectX 11/12)
- `assets/shaders/compiled/fs_simple.bin` - Compiled fragment shader (DirectX 11/12)

### Shader Compiler
- `external/bgfx/tools/shaderc.exe` - bgfx shader compiler (included in repository)

## Rendering Improvements

The cube now appears much whiter due to:
1. Higher ambient lighting (60% instead of 30%)
2. Stronger diffuse lighting (120% instead of 70%)
3. Minimum brightness clamp ensuring at least 50% visibility
4. Base material color already set to pure white (1.0, 1.0, 1.0)

## Build System Integration

The CMake build system now:
- Automatically detects shaderc at `external/bgfx/tools/shaderc.exe`
- Creates shader compilation commands with proper dependencies
- Ensures shaders are recompiled when source files change
- Runs shader compilation before linking the main executable

## Troubleshooting

If shaders don't compile:
1. Verify `external/bgfx/tools/shaderc.exe` exists
2. Run `cmake --preset x64-debug` to reconfigure
3. Run `cmake --build out/build/x64-debug --target CompileShaders` to manually trigger compilation
4. Check the output for any compilation errors

If the cube is still too dark:
- Adjust the lighting values in `assets/shaders/fs_simple.sc`
- Increase the `ambient` multiplier (currently 0.6)
- Increase the `diffuse` multiplier (currently 1.2)
- Adjust the minimum brightness clamp (currently 0.5)

## Next Steps

You can now:
1. Run the application - the cube should appear much brighter/whiter
2. Modify shaders in `assets/shaders/*.sc` and rebuild to see changes
3. Add more shader variants for different materials and lighting models
