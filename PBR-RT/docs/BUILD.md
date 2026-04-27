# Anito Engine - Build Guide

**AAA Unified Build System**  
**DLSU GAME Lab - Project Anito**

---

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Quick Start](#quick-start)
3. [Build System](#build-system)
4. [Build Configurations](#build-configurations)
5. [Troubleshooting](#troubleshooting)
6. [Advanced Usage](#advanced-usage)

---

## Prerequisites

### Required Software

1. **Visual Studio 2026** (Community, Professional, or Enterprise)
   - Download: https://visualstudio.microsoft.com/downloads/
   - **Required components:**
     - Desktop development with C++
     - Windows 10/11 SDK
   - Version: 18.x required for proper CMake integration

2. **CMake 3.10 or newer** (4.2.3-msvc3 recommended)
   - Download: https://cmake.org/download/
   - **Important:** Add CMake to PATH during installation
   - Included with Visual Studio 2026

3. **Git**
   - Download: https://git-scm.com/download/win
   - Required for cloning submodules

### System Requirements

- **OS:** Windows 10/11 (macOS/Linux support planned)
- **Platform:** x64
- **C++ Standard:** C++20
- **CMake Generator:** Visual Studio 18 2026

---

## Quick Start

### Option 1: New Hardware Setup (Recommended)

For brand new machines, use our automated setup:

```powershell
# 1. Clone the repository
git clone https://github.com/NeilDG/GDENG03-Engine
cd GDENG03-Engine/PBR-RT

# 2. First-time setup (initializes submodules and configures CMake)
Build.bat setup

# 3. Build the engine
Build.bat

# 4. Run the engine
Build.bat run
```

**That's it!** The setup command handles all dependencies automatically.

### Option 2: Visual Studio Integration

If you prefer working entirely in Visual Studio:

1. **Close Visual Studio** if currently open
2. **Reopen Visual Studio**
3. Select **File > Open > Folder**
4. Navigate to `PBR-RT/` and open it
5. Wait for "CMake generation finished" in the Output window
6. Select **Build > Build All** (or press `Ctrl+Shift+B`)
7. Select **Debug > Start Without Debugging** (`Ctrl+F5`)

---

## Build System

### Unified Build System

Anito Engine uses a **single unified build script** following AAA studio best practices (similar to Unreal Engine's RunUAT, CryEngine's build tools):

```
Build.bat [command] [options]
```

### Available Commands

```bash
# Setup & Configuration
Build.bat setup          # First-time setup: init submodules, configure CMake
Build.bat check          # Verify dependencies and build system
Build.bat validate       # Validate Build.bat and CMakeLists.txt sync

# Building
Build.bat                # Build with default config (Release)
Build.bat build          # Same as above
Build.bat build Debug    # Build debug configuration
Build.bat build Release  # Build release configuration
Build.bat rebuild        # Clean and rebuild
Build.bat clean          # Clean build artifacts

# Running
Build.bat run            # Build and run (Release)
Build.bat run Debug      # Build and run (Debug)

# Utilities
Build.bat shaderc        # Helper for shader compiler setup
Build.bat help           # Show all available commands
```

### Build Options

```bash
# Specify configuration
Build.bat build --config Debug
Build.bat build --config Release

# Verbose output
Build.bat build --verbose

# Combined options
Build.bat build --config Debug --verbose
```

---

## Build Configurations

Anito Engine supports standard Visual Studio configurations:

| Configuration | Description | Use Case |
|---------------|-------------|----------|
| **Debug** | Full debugging symbols, no optimization | Development, debugging |
| **Release** | Full optimization, minimal debug info | Performance testing, shipping |
| **RelWithDebInfo** | Release with debug symbols | Profiling, release debugging |
| **MinSizeRel** | Optimized for binary size | Mobile, embedded targets |

**Recommended workflow:**
- Use **Debug** for daily development
- Use **Release** for performance testing and profiling
- Use **RelWithDebInfo** when debugging performance issues

---

## Troubleshooting

### Common Issues & Solutions

#### ❌ Issue: "Cannot find bgfx/bgfx.h"
**Cause:** Missing or incomplete bgfx submodule  
**Solution:**
```powershell
Build.bat setup    # Re-initialize submodules
```

#### ❌ Issue: "Cannot find glm/glm.hpp"
**Cause:** Missing GLM submodule  
**Solution:**
```powershell
git submodule update --init --recursive external/glm
Build.bat setup
```

#### ❌ Issue: "CMake configuration failed"
**Solution:**
```powershell
# Option 1: Via Build.bat
Build.bat clean
Build.bat setup

# Option 2: Via Visual Studio
# 1. Close Visual Studio
# 2. Delete the build/ folder
# 3. Reopen Visual Studio
# 4. Let it reconfigure automatically
```

#### ❌ Issue: "x86/x64 mismatch"
**Cause:** Wrong architecture selected  
**Solution:**
```powershell
# Ensure Visual Studio is using x64:
# 1. Project > CMake Settings for AnitoEngine
# 2. Set Configuration to "x64-Debug" or "x64-Release"
# 3. Delete cache and reconfigure
```

#### ❌ Issue: "shaderc.exe not found" during build
**Cause:** Shader compiler not built yet  
**Solution:** See [Shader Compilation Guide](SHADER_PIPELINE.md)

#### ❌ Issue: Build.bat says "CMake version too old"
**Solution:** Upgrade CMake to 3.10+ (download from cmake.org)

#### ❌ Issue: "Generator 'Visual Studio 18 2026' not found"
**Solution:** Install Visual Studio 2026 or update Build.bat to use your VS version

---

## Advanced Usage

### Build System Configuration

The build system is configured via:

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Core CMake configuration |
| `Build.bat` | Unified build interface |
| `engine_config.ini` | Runtime configuration |

#### CMake Options

You can customize the build via CMake variables:

```powershell
cmake -B build -G "Visual Studio 18 2026" -A x64 `
  -DCMAKE_CXX_STANDARD=20 `
  -DANITO_BUILD_SAMPLES=ON `
  -DANITO_USE_VULKAN=ON `
  -DANITO_COMPILE_SHADERS=ON
```

### Build Validation

To ensure Build.bat and CMakeLists.txt are synchronized:

```powershell
Build.bat validate
```

This checks:
- ✅ CMake minimum version consistency
- ✅ C++ standard consistency  
- ✅ Generator specification
- ✅ Platform specification
- ✅ Build directory paths

### Manual CMake Workflow (Advanced)

If you need fine-grained control:

```powershell
# Configure
cmake -B build -G "Visual Studio 18 2026" -A x64

# Build
cmake --build build --config Release

# Install (if configured)
cmake --install build --prefix install/
```

---

## Build Output

Successful builds create:

```
build/
├── bin/
│   ├── Debug/
│   │   └── AnitoEngine.exe       # Debug executable
│   └── Release/
│       └── AnitoEngine.exe       # Release executable
├── lib/                          # Static libraries (if any)
└── CMakeFiles/                   # CMake internals
```

### Expected Output

When you run the engine, you should see:

1. **1280x720 window** titled "Anito Engine"
2. **Console log** showing initialization
3. **Rendered scene** with PBR materials and IBL
4. **ImGui overlay** showing performance stats (if enabled)

---

## Next Steps

After successful build:

1. ✅ **Run the engine**: `Build.bat run`
2. 📖 **Read the [Shader Pipeline Guide](SHADER_PIPELINE.md)** to understand shader workflow
3. 🎮 **Explore test scenes**: Press `SPACE` to cycle through PBR test scenes
4. 🔧 **Check [Runtime Configuration](RUNTIME_CONFIG.md)** for engine settings
5. 📊 **Enable profiling**: See [Profiling Guide](PROFILING.md)

---

## Dependencies Reference

All external dependencies are in `external/`:

| Library | Purpose | Status |
|---------|---------|--------|
| **bgfx** | Rendering abstraction | ✅ Required |
| **GLFW** | Window/input management | ✅ Required |
| **GLM** | Math library | ✅ Required |
| **Bullet Physics** | Physics simulation | ✅ Required |
| **ImGui** | Immediate mode GUI | ✅ Required |
| **ImGuizmo** | 3D gizmo manipulation | ✅ Required |
| **STB** | Image loading (HDR, PNG) | ✅ Required |

Dependencies are managed as Git submodules and initialized by `Build.bat setup`.

---

## Support

- **Documentation**: See `/docs` directory
- **Issues**: GitHub issue tracker
- **Project Lead**: Neil Patrick Del Gallego, Ph.D.
- **Institution**: DLSU GAME Lab

For build system issues, check:
1. `Build.bat validate` output
2. Visual Studio CMake logs: View > Output > CMake
3. Build logs in `build/` directory
