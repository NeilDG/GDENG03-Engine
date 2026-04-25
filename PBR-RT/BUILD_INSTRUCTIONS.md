# Anito Engine - Build Instructions for Windows

This guide will help you build Anito Engine on any Windows machine with minimal setup.

## Prerequisites

1. **Visual Studio 2026** (Community, Professional, or Enterprise) with C++ support
   - Download from: https://visualstudio.microsoft.com/downloads/
   - Required components:
     - Desktop development with C++
     - Windows SDK
   - **Note:** Visual Studio 2026 (version 18.x) is required for proper CMake integration

2. **CMake 3.10 or newer** (4.2.3-msvc3 recommended)
   - Download from: https://cmake.org/download/
   - Make sure to add CMake to PATH during installation

3. **Git**
   - Download from: https://git-scm.com/download/win
   - Required for cloning submodules

## Build System Configuration

The Anito Engine uses a **unified build system** similar to AAA game engines like Unreal Engine.

### System Specifications
- **CMake Generator:** Visual Studio 18 2026
- **Platform:** x64
- **C++ Standard:** C++20
- **Minimum CMake:** 3.10
- **Build Directory:** `build/`
- **Output Directory:** `build/bin/<Configuration>/`

### Synchronized Configuration
`Build.bat` is synchronized with `CMakeLists.txt` to ensure consistent builds:
- ✅ CMake version validation (>= 3.10)
- ✅ Explicit generator specification (Visual Studio 18 2026)
- ✅ Platform enforcement (x64)
- ✅ C++ standard enforcement (C++20)
- ✅ Multi-configuration support (Debug, Release, RelWithDebInfo, MinSizeRel)

## Quick Start (First Time Setup)

### Step 1: Verify Requirements

```bash
Build.bat check
```

This will verify:
- Git installation
- CMake installation and version
- Visual Studio 2026 availability

### Step 2: Setup (First Time Only)

```bash
Build.bat setup
```

This will:
- Validate CMake version (>= 3.10)
- Initialize and update all Git submodules
- Configure CMake with Visual Studio 18 2026 generator
- Set x64 platform
- Enable C++20 standard
- Configure build options (IBL, Vulkan, shader compilation)

### Step 3: Build

```bash
Build.bat build
```

Or explicitly specify configuration:
```bash
Build.bat build Release
Build.bat build Debug
```

### Step 4: Run

```bash
Build.bat run
```

Or run specific configuration:
```bash
Build.bat run Debug
Build.bat run Release
```

## All Commands

The unified `Build.bat` script provides all build operations:

### Core Commands
```bash
Build.bat setup              # Initial setup (run once on new hardware)
Build.bat build [config]     # Build the project (default: Release)
Build.bat rebuild [config]   # Clean and rebuild
Build.bat run [config]       # Build and run the engine
Build.bat clean              # Remove all build artifacts
Build.bat check              # Verify dependencies
Build.bat shaderc            # Build shader compiler helper
Build.bat help               # Show usage information
```

### Build Configurations
- **Release** (default) - Optimized build for production
- **Debug** - Debug symbols, no optimization
- **RelWithDebInfo** - Release with debug information
- **MinSizeRel** - Minimum size release build

### Examples
```bash
# Build in Release mode (default)
Build.bat

# Build in Debug mode
Build.bat build Debug

# Build and run Debug version
Build.bat run Debug

# Clean and rebuild Release
Build.bat rebuild Release

# Build with verbose output
Build.bat build --verbose
```

## Shader Compilation (Optional but Recommended)

Anito Engine uses bgfx shaders which can be automatically compiled using the `shaderc` tool.

### Quick Method

```bash
# Build shaderc from source
tools\build_shaderc.bat

# Rebuild to enable shader compilation
Build.bat rebuild
```

### Alternative: Download Prebuilt

1. Download from: https://github.com/bkaradzic/bgfx/releases
2. Extract `shaderc.exe` to `tools\shaderc.exe`
3. Run: `Build.bat rebuild`

### Skip Shader Compilation

If you already have compiled shaders:

```bash
cmake -B build -DANITO_COMPILE_SHADERS=OFF
Build.bat build
```

## Build Options

Customize the build with options:

```bash
# Debug build
Build.bat build --config Debug

# Verbose output
Build.bat build --verbose

# Disable shader compilation
cmake -B build -DANITO_COMPILE_SHADERS=OFF
Build.bat build

# Strict mode - fail if shaderc not found
cmake -B build -DANITO_FAIL_ON_MISSING_SHADERC=ON
Build.bat build
```

## Troubleshooting

### "shaderc.exe not found"

This is a warning, not an error. The build will succeed but shaders won't be compiled automatically.

**Solution**: Run `tools\build_shaderc.bat` or download prebuilt shaderc as described above.

### "Git submodules not initialized"

**Solution**: Run these commands:
```bash
git submodule update --init --recursive
```

### "CMake configuration failed"

**Solution**: 
1. Verify Visual Studio is installed with C++ support
2. Ensure CMake is in your PATH: `cmake --version`
3. Check the error message for missing dependencies

### "MSBuild not found" when building shaderc

**Solution**:
1. Open the generated Visual Studio solution manually:
   - Navigate to `external\bgfx\.build\projects\vs2022\`
   - Open `bgfx.sln`
   - Build the `shaderc` project
   - Copy the resulting `shaderc.exe` to `tools\shaderc.exe`

### Build errors related to runtime libraries

This usually happens with GLFW. The CMakeLists.txt has been configured to handle this automatically, but if you still see issues:

**Solution**: Clean and rebuild:
```bash
cmake --build build --target clean
cmake -B build --fresh
cmake --build build --config Release
```

## Directory Structure

```
AnitoEngine/
├── build/              # CMake build output (generated)
├── assets/             # Game assets and shaders
│   └── shaders/
│       ├── *.sc        # Shader source files
│       └── compiled/   # Compiled shader binaries (generated)
├── external/           # Third-party dependencies (submodules)
│   ├── bgfx/
│   ├── bgfx.cmake/
│   ├── glfw/
│   ├── glm/
│   ├── bullet3/
│   ├── imgui/
│   └── ImGuizmo/
├── src/                # Engine source code
├── tools/              # Build tools and scripts
│   └── shaderc.exe     # Shader compiler (generated or copied)
└── CMakeLists.txt      # Main build configuration
```

## Switching to New Hardware

When setting up on a new Windows machine:

```bash
# 1. Clone repository
git clone <repo-url> AnitoEngine
cd AnitoEngine

# 2. Setup
Build.bat setup

# 3. Build
Build.bat

# 4. (Optional) Build shaderc
tools\build_shaderc.bat
Build.bat rebuild

# 5. Run
Build.bat run
```

The CMake configuration automatically detects your Visual Studio version and system architecture.

## Advanced: Manual CMake Configuration

If you prefer manual configuration:

```bash
# Configure for Visual Studio 2022 (64-bit)
cmake -B build -G "Visual Studio 17 2022" -A x64

# Configure for Visual Studio 2019 (64-bit)
cmake -B build -G "Visual Studio 16 2019" -A x64

# Build
cmake --build build --config Release

# Or specify configuration
cmake --build build --config Debug
```

## Support

For issues or questions:
- Check the troubleshooting section above
- Review CMake output for specific error messages
- Ensure all prerequisites are installed correctly

---

**DLSU GAME Lab - AAA Game Engine Development**
Principal Investigator: Neil Patrick Del Gallego, Ph.D.
DOST-PCIEERD Project Anito
