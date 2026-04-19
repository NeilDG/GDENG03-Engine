# Quick Start Guide - Shader Compilation Setup

## Option 1: Download Prebuilt Tools (Recommended - Fastest)

### Step 1: Download bgfx Tools
1. Go to: https://github.com/bkaradzic/bgfx/releases
2. Download the latest release (e.g., `bgfx-windows-x64-Release.zip`)
3. Extract the archive

### Step 2: Copy shaderc.exe
From the extracted folder, copy `shaderc.exe` to:
```
X:\GithubProjects\GDENG03-Engine-PBR-RT\external\bgfx\.build\win64_vs2022\bin\shadercRelease.exe
```

Note: You may need to create the directories first:
```powershell
New-Item -ItemType Directory -Force -Path "external\bgfx\.build\win64_vs2022\bin"
```

### Step 3: Compile Shaders
```powershell
powershell -ExecutionPolicy Bypass -File tools\compile_shaders.ps1
```

### Step 4: Run the Engine
```powershell
.\out\build\x64-debug\bin\AnitoEngine.exe
```

You should see a rotating red cube!

---

## Option 2: Build Tools from Source (If prebuilt not available)

### Step 1: Download Genie
1. Go to: https://github.com/bkaradzic/genie/releases
2. Download `genie-windows-x64.zip`
3. Extract `genie.exe`

### Step 2: Place Genie
Copy `genie.exe` to:
```
external\bx\tools\bin\windows\genie.exe
```

### Step 3: Generate Build Files
```powershell
cd external\bgfx
..\bx\tools\bin\windows\genie.exe --with-tools vs2022
```

### Step 4: Build shaderc
1. Open `.build\projects\vs2022\bgfx.sln` in Visual Studio
2. Set configuration to **Release** and platform to **x64**
3. Build the `shaderc` project
4. The executable will be in `.build\win64_vs2022\bin\shadercRelease.exe`

### Step 5: Compile Shaders
```powershell
cd ..\..
powershell -ExecutionPolicy Bypass -File tools\compile_shaders.ps1
```

### Step 6: Run the Engine
```powershell
.\out\build\x64-debug\bin\AnitoEngine.exe
```

---

## Expected Output After Shader Compilation

When you run `compile_shaders.ps1`, you should see:
```
========================================
Anito Engine Shader Compiler
========================================

Shaderc: external\bgfx\.build\win64_vs2022\bin\shadercRelease.exe
Source:  assets\shaders
Output:  assets\shaders\compiled

Compiling shaders...

Compiling: assets\shaders\vs_simple.sc -> vs_simple.bin (vertex)
  SUCCESS
Compiling: assets\shaders\fs_simple.sc -> fs_simple.bin (fragment)
  SUCCESS

========================================
Shader compilation complete!
========================================
```

Compiled shaders will be in: `assets\shaders\compiled\`

---

## When You Run the Engine

### Expected Console Output:
```
==================================================
  Anito Engine - Project Anito (DOST-PCIEERD)
  DLSU GAME Lab - AAA Game Engine Development
  Principal Investigator: Neil Patrick Del Gallego, Ph.D.
==================================================
Anito Renderer initialized successfully
Renderer: Vulkan
AnitoGameObjectManager initialized

[Scene Setup] Creating test scene...
[Scene Setup] Loading shader...
[AnitoShader] Successfully loaded shader: SimpleShader
[Scene Setup] Creating material...
[Scene Setup] Generating cube mesh...
[Scene Setup] Creating cube GameObject...
[Scene Setup] Setting up camera...
[Scene Setup] Test scene created successfully!
  - Cube at (0, 0, -5)
  - Camera at (0, 2, 5)
==================================================

[Engine] Starting main loop...
```

### Expected Visual Output:
- Window: 1280x720, titled "Anito Engine - Deferred PBR Renderer"
- Background: Dark gray (#303030)
- Object: Red-ish cube rotating smoothly
- Lighting: Directional light creating visible shading

---

## Troubleshooting

### "Failed to load shader!"
- **Cause**: Shaders not compiled
- **Fix**: Run shader compilation script (see above)

### "shaderc not found"
- **Cause**: shaderc.exe not in the expected location
- **Fix**: Download prebuilt tools or build from source

### Black screen with no cube
- **Cause**: Shader compilation may have failed silently
- **Fix**: Check `assets\shaders\compiled\` for .bin files
- Rerun compilation with verbose output

### Application crashes on startup
- **Cause**: Missing dependencies or corrupt shader files
- **Fix**: 
  1. Rebuild the project
  2. Delete `assets\shaders\compiled\` and recompile
  3. Check console output for error messages

---

## Controls (Future Implementation)

Currently no interactive controls - the cube rotates automatically.

Future controls will include:
- **Mouse**: Orbit camera
- **WASD**: Free-fly movement
- **Mouse Wheel**: Zoom
- **ESC**: Exit

---

## Next Steps After Successful Render

Once you see the rotating cube:

1. ✅ **Phase 3A Complete!**
2. 📋 Move to **Phase 3B**: Deferred Rendering
   - Implement G-Buffer
   - Multiple render targets
   - Deferred lighting pass
3. 📋 Continue to **Phase 3C**: Advanced Lighting
   - Point/spot lights
   - Shadow mapping
   - Cascaded shadows

---

**Note**: The first visual output is a major milestone! Once you see the cube rendering, all the foundation is in place for advanced rendering techniques.

**Good luck!** 🚀
