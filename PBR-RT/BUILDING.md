# Building Anito Engine

## Prerequisites

- **Windows 10/11**
- **Visual Studio 2022** or newer (2026 currently installed)
- **CMake 3.10+** (included with Visual Studio)
- **Git**

## Step-by-Step Build Instructions

### Step 1: Ensure Dependencies Are Cloned

All dependencies should already be in `external/`:
- ✅ glm
- ✅ glfw
- ✅ bgfx (with submodules)
- ✅ bgfx.cmake
- ✅ bullet3
- ✅ imgui
- ✅ ImGuizmo

If any are missing, see README.md for clone commands.

### Step 2: Build in Visual Studio (RECOMMENDED)

1. **Close Visual Studio** if it's currently open
2. **Reopen Visual Studio**
3. Select **File > Open > Folder**
4. Navigate to `X:\GithubProjects\GDENG03-Engine-PBR-RT` and open it
5. Visual Studio will automatically detect `CMakeLists.txt` and configure the project
6. Wait for "CMake generation finished" message in the Output window
7. Select **Build > Build All** (or press `Ctrl+Shift+B`)
8. Once built, select **Debug > Start Without Debugging** (`Ctrl+F5`) or **Debug > Start Debugging** (`F5`)

### Step 3: If Configuration Fails

If Visual Studio shows CMake errors:

1. **Delete Cache**: Project > Delete Cache and Reconfigure
2. **Check Output**: View > Output, select "CMake" from dropdown
3. **Verify Dependencies**: Ensure all folders exist in `external/`

### Common Issues & Solutions

#### Issue: "Cannot find bgfx/bgfx.h"
**Solution**: Ensure `external/bgfx` and `external/bgfx.cmake` both exist

#### Issue: "Cannot find glm/glm.hpp"
**Solution**: Ensure `external/glm` exists

#### Issue: "CMake configuration failed"
**Solution**: 
1. Close Visual Studio
2. Delete `out/` folder if it exists
3. Reopen Visual Studio and let it reconfigure

#### Issue: "x86/x64 mismatch"
**Solution**: 
1. Project > CMake Settings for AnitoEngine
2. Ensure Configuration type is set to "x64-Debug" or "x64-Release"
3. Delete cache and reconfigure

## Build Output

Successful build will create:
- `out/build/x64-debug/bin/AnitoEngine.exe` (Debug)
- `out/build/x64-release/bin/AnitoEngine.exe` (Release)

## Running the Engine

From Visual Studio:
- Press `F5` (Debug) or `Ctrl+F5` (Run without debugging)

From Command Line:
```bash
.\out\build\x64-debug\bin\AnitoEngine.exe
```

## Build Configurations

- **x64-Debug**: Full debugging symbols, no optimization
- **x64-Release**: Optimized, minimal debugging info
- **x86-Debug**: 32-bit debug (if needed)
- **x86-Release**: 32-bit release (if needed)

**Recommended**: Use x64-Debug for development, x64-Release for performance testing

## Next Steps After Build

Once the engine runs successfully:
- You should see a 1280x720 window with "Anito Engine" title
- Console will show initialization messages
- Window will have a dark gray background (no rendering yet - shaders not implemented)

See TODO.md for next development tasks!
