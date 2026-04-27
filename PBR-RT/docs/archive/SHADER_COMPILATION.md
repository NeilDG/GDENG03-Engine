# Shader Compilation for Anito Engine

## The Problem
The cube appears black because the shader compilation is not automatic. The shader source files (`.sc`) need to be compiled to binary shaders (`.bin`) using **shaderc** before the engine can use them.

## Quick Fix - Manual Shader Compilation

### Step 1: Download shaderc
1. Go to: https://github.com/bkaradzic/bgfx/releases/latest
2. Download the Windows tools package (look for `bgfx-windows-x64.zip` or similar)
3. Extract the archive
4. Find `shaderc.exe` inside (usually in a `bin` or `tools` folder)
5. Copy `shaderc.exe` to: `X:\GithubProjects\GDENG03-Engine-PBR-RT\tools\shaderc\shaderc.exe`

### Step 2: Compile Shaders
Open PowerShell in the project root and run:
```powershell
powershell -ExecutionPolicy Bypass -File tools/compile_shaders.ps1
```

This will compile:
- `assets/shaders/vs_simple.sc` → `assets/shaders/compiled/vs_simple.bin`
- `assets/shaders/fs_simple.sc` → `assets/shaders/compiled/fs_simple.bin`

### Step 3: Rebuild and Run
1. Rebuild the project in Visual Studio
2. Run the engine

The cube should now render as **white/light gray** instead of black!

## What Changed in the Shaders

The fragment shader (`fs_simple.sc`) now has:
- **Higher ambient lighting** (0.3 instead of 0.1) for better visibility
- **Corrected light direction** calculation
- **Vertex color fallback** to white if no vertex colors are provided

## Future: Automatic Shader Compilation

Ideally, shader compilation should be integrated into the CMake build process. This would require:
1. Downloading shaderc automatically during CMake configuration
2. Adding custom build targets for shader compilation
3. Making the executable depend on compiled shaders

This is on the TODO list but for now, manual compilation works fine.

## Troubleshooting

**Problem**: Shaders still compile but cube is black
- Make sure you rebuild the C++ project after compiling shaders
- Check that vertex colors are set to white (0xFFFFFFFF) in mesh data
- Verify light direction is negated correctly in C++ code

**Problem**: Cannot find shaderc.exe
- Try building bgfx tools manually:
  ```
  cd external/bgfx
  ../bx/tools/bin/windows/genie --with-tools vs2022
  ```
  Then build `.build/projects/vs2022/bgfx.sln` in Release mode

**Problem**: Shader compilation fails
- Check that `assets/shaders/varying.def.sc` exists
- Verify bgfx include path: `external/bgfx/src`
- Make sure shader source files have no syntax errors
