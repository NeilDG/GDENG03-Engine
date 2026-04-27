# SETUP: Shader Compilation

The cube appears black because shaders need to be compiled. Here's how to fix it:

## Quick Setup (One-Time)

### Option 1: Build shaderc from bgfx source (RECOMMENDED)

Since you already have bgfx source code, build shaderc directly:

1. **Run the build script**:
   ```cmd
   tools\build_shaderc.bat
   ```

   This will:
   - Generate Visual Studio projects using GENie
   - Build shaderc in Release mode
   - Copy `shaderc.exe` to `tools/shaderc/`

2. **Reconfigure CMake** (in Visual Studio):
   - `Project` → `Delete Cache and Reconfigure`
   - Or from command line: `cmake --preset x64-debug`

3. **Build** - Shaders will now compile automatically before each build!

### Option 2: Manual shader compilation (if Option 1 doesn't work)

If building shaderc fails, you can compile shaders manually each time:

```cmd
compile_shaders.bat
```

Then rebuild the project.

## How It Works

Once shaderc.exe is in place:
- CMake will detect it automatically
- Shaders in `assets/shaders/*.sc` will be compiled to `assets/shaders/compiled/*.bin`
- This happens automatically as a pre-build step
- The cube will render as white/light gray instead of black!

## Manual Shader Compilation (Alternative)

If you don't want automatic compilation, you can compile shaders manually:

```cmd
compile_shaders.bat
```

Or:

```powershell
powershell -ExecutionPolicy Bypass -File tools/compile_shaders.ps1
```

Then rebuild the project.

## What Changed

The shader fixes include:
- ✅ Added white vertex colors to mesh data (fixes black multiplication)
- ✅ Increased ambient lighting (0.3 instead of 0.1)
- ✅ Corrected light direction handling
- ✅ Set material to white base color for visibility

All these changes are in the source shaders but need to be **compiled** to take effect!
