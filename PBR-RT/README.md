# Anito Engine - Project Anito (DOST-PCIEERD)

**DLSU GAME Lab - AAA Game Engine Development**  
**Principal Investigator:** Neil Patrick Del Gallego, Ph.D.  
**Project Period:** June 2025 – June 2027

## Overview

Anito Engine is a modular, high-performance game engine designed for AAA visual fidelity on PC and Android platforms. It features:

- **Deferred PBR Rendering** with physically-based materials
- **Real-time Shadows** with shadow mapping
- **GPU-based Object Picking** (industry-standard approach)
- **bgfx Rendering Abstraction** (cross-platform Vulkan/DX12/Metal)
- **Bullet Physics Integration**
- **Component-based GameObject System**
- **ImGui + ImGuizmo** for editor tools

## Architecture

Following modern game engine best practices:
- Entity-Component-System (ECS) core
- Deferred rendering pipeline
- Data-oriented design
- Modular subsystems

## Dependencies

### Required External Libraries

1. **bgfx** - Rendering abstraction layer
2. **GLFW** - Window and input management  
3. **GLM** - Mathematics library
4. **Bullet Physics** - Physics simulation
5. **ImGui** - Immediate mode GUI
6. **ImGuizmo** - 3D gizmo manipulation

## Quick Start (Windows)

### Simple Setup on New Hardware

```bash
# 1. Setup (first time only - initializes submodules and configures CMake)
Build.bat setup

# 2. Build
Build.bat

# 3. Run
Build.bat run
```

That's it! The unified build system handles everything.

### All Available Commands

```bash
Build.bat setup      # Initial setup (run once on new hardware)
Build.bat build      # Build the project (default command)
Build.bat rebuild    # Clean and build
Build.bat clean      # Remove build artifacts
Build.bat check      # Verify dependencies
Build.bat run        # Build and run the engine
Build.bat help       # Show all commands

# Options
Build.bat build --config Debug    # Build in Debug mode
Build.bat build --verbose         # Detailed output
```

### Shader Compiler (Optional)

To enable automatic shader compilation:

```bash
# Build shaderc from source
tools\build_shaderc.bat

# Then rebuild
Build.bat rebuild
```

### What It Does

The build system automatically:
- Checks for Git, CMake, and Visual Studio
- Initializes all Git submodules
- Configures CMake for your hardware
- Detects your Visual Studio version
- Builds the project

### Detailed Documentation

- **[BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)** - Complete build guide with troubleshooting
- **[SHADER_COMPILATION_README.md](SHADER_COMPILATION_README.md)** - Shader compilation details

### Manual CMake (Alternative)

If you prefer direct CMake control:

```bash
# Clone with submodules
git clone --recursive <repository-url>

# Or initialize submodules
git submodule update --init --recursive

# Configure and build
cmake -B build
cmake --build build --config Release

# Run
.\build\bin\Release\AnitoEngine.exe
```

## Project Structure

```
GDENG03-Engine-PBR-RT/
├── external/              # Third-party dependencies
│   ├── bgfx/
│   ├── glfw/
│   ├── glm/
│   ├── bullet3/
│   ├── imgui/
│   └── ImGuizmo/
├── src/
│   ├── Math/             # Vector and Matrix classes
│   ├── Renderer/         # Core rendering (bgfx wrappers)
│   │   ├── Deferred/     # Deferred rendering pipeline
│   │   └── Lighting/     # Lighting and shadows
│   ├── GameObjects/      # GameObject and Component system
│   ├── Components/       # Built-in components
│   ├── Window/           # Window management
│   ├── Input/            # Input system
│   └── main.cpp          # Entry point
├── assets/               # Shaders, models, textures
└── CMakeLists.txt
```

## Current Implementation Status

### ✅ Phase 1: Core Foundation (COMPLETE)
- [x] bgfx initialization and window creation
- [x] Math library wrappers (Vector3D, Matrix4x4)
- [x] Vertex/Index buffer abstractions
- [x] Main engine loop

### 🚧 Phase 2: GameObject System (IN PROGRESS)
- [ ] AnitoGameObject base class
- [ ] AnitoComponent system
- [ ] AnitoGameObjectManager
- [ ] Transform component
- [ ] Camera component
- [ ] MeshRenderer component

### 📋 Phase 3: Deferred Rendering (PLANNED)
- [ ] G-Buffer implementation
- [ ] PBR material system
- [ ] Lighting pass
- [ ] Shadow mapping
- [ ] Post-processing

### 📋 Phase 4: Object Picking (PLANNED)
- [ ] GPU-based ID rendering
- [ ] Mouse raycast system
- [ ] Selection highlighting

### 📋 Phase 5: Editor Tools (PLANNED)
- [ ] ImGui integration
- [ ] ImGuizmo transform gizmos
- [ ] Object hierarchy panel
- [ ] Properties panel

## Rendering Style

**Pipeline:** Deferred Rendering  
**Lighting Model:** PBR (Metallic-Roughness)  
**Shadow Technique:** Shadow Maps with Shadow Atlas  
**Object Picking:** GPU-based ID rendering (Unity/Unreal style)

## Development Notes

This implementation follows the architecture patterns from the reference DirectX 11 engine (Part 12 - IMGuizmo Integration) but adapted for:
- bgfx instead of DirectX 11
- Deferred rendering instead of forward rendering
- Bullet Physics instead of ReactPhysics3D
- GLM for math instead of custom implementation

## Agent Responsibilities

This implementation is being developed following the **Anito Renderer** agent persona with coordination from **Anito Architect**. See `agents.md` for full multi-agent development system.

## License

DOST-PCIEERD Project Anito  
De La Salle University - GAME Lab

---

**Next Steps:**
1. Implement GameObject/Component system
2. Create camera system with orbit controls
3. Set up deferred rendering pipeline
4. Add basic PBR lighting
5. Implement shadow mapping
6. Add GPU-based object picking
7. Integrate ImGuizmo for transform manipulation
