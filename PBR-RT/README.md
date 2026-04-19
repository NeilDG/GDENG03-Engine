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

## Quick Start

### 1. Clone Dependencies

The external dependencies need to be cloned into the `external/` directory:

```bash
# Create external directory if it doesn't exist
mkdir external
cd external

# Clone dependencies
git clone https://github.com/g-truc/glm.git
git clone https://github.com/glfw/glfw.git
git clone --recursive https://github.com/bkaradzic/bgfx.git
git clone https://github.com/bkaradzic/bgfx.cmake.git
git clone https://github.com/bulletphysics/bullet3.git
git clone https://github.com/ocornut/imgui.git
git clone https://github.com/CedricGuillemet/ImGuizmo.git
cd ..
```

**Note:** All dependencies have been cloned if you ran the steps above or if the `external/` folder contains these libraries.

### 2. Build with Visual Studio

The project uses CMake and is configured for Visual Studio:

**Option A: Visual Studio IDE (Recommended)**
1. Open Visual Studio 2022 or newer
2. Select "Open a local folder"
3. Navigate to and open `X:\GithubProjects\GDENG03-Engine-PBR-RT`
4. Visual Studio will automatically configure CMake
5. Select "Build > Build All" or press Ctrl+Shift+B
6. Run the project

**Option B: Command Line**
```bash
# Configure (first time only)
cmake -B build -G Ninja

# Build
cmake --build build

# Run
.\build\bin\AnitoEngine.exe
```

### 3. Troubleshooting

If you encounter build errors:
- Ensure all dependencies are in `external/` directory
- Delete the `out/` or `build/` folder and reconfigure
- In Visual Studio: Project > Delete Cache and Reconfigure
- Ensure you have Visual Studio 2022 or newer with C++ Desktop Development workload

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
