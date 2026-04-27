# Anito Engine - Project Anito (DOST-PCIEERD)

**DLSU GAME Lab - AAA Game Engine Development**  
**Principal Investigator:** Neil Patrick Del Gallego, Ph.D.  
**Project Period:** June 2025 – June 2027

---

## Overview

Anito Engine is a modular, high-performance game engine designed for **AAA visual fidelity** on **PC** and **Android** platforms.

### Key Features

- ✅ **Physically-Based Rendering (PBR)** - Metallic/roughness workflow
- ✅ **Image-Based Lighting (IBL)** - HDR environment maps for realistic lighting
- ✅ **Deferred Rendering** - Optimized for complex lighting scenarios
- ✅ **bgfx Rendering Abstraction** - Cross-platform (Vulkan/DX11/DX12/Metal)
- ✅ **Comprehensive Profiling** - CPU, GPU, memory, crash reporting
- ✅ **Component-based Architecture** - Flexible GameObject system
- ✅ **Bullet Physics Integration** - Rigid body dynamics
- ✅ **ImGui + ImGuizmo** - Editor UI and gizmo tools

### Current Status

**Phase 2 Complete** - Core PBR rendering with IBL  
**Next Phase** - Shadow mapping and advanced lighting

See [ROADMAP.md](docs/ROADMAP.md) for full development plan.

---

## Quick Start

### 1. Clone & Setup

```powershell
# Clone repository
git clone https://github.com/NeilDG/GDENG03-Engine
cd GDENG03-Engine/PBR-RT

# First-time setup (initializes submodules, configures CMake)
Build.bat setup
```

### 2. Build & Run

```powershell
# Build (default: Release)
Build.bat

# Run the engine
Build.bat run

# Or build and run in one command
Build.bat run Debug
```

### 3. Test PBR Rendering

Once running:
- **SPACE** - Cycle through test scenes
- **Z** - Toggle IBL on/off
- **ESC** - Quit

---

## Build System

Anito Engine uses a **unified build system** following AAA studio practices:

```powershell
Build.bat [command] [options]
```

### Commands

| Command | Description |
|---------|-------------|
| `setup` | First-time setup (init submodules, configure) |
| `build` | Build the project (default command) |
| `rebuild` | Clean and rebuild |
| `clean` | Remove build artifacts |
| `check` | Verify dependencies |
| `run` | Build and run the engine |
| `validate` | Check build configuration sync |
| `help` | Show all commands |

### Options

```powershell
--config Debug        # Build in Debug mode
--config Release      # Build in Release mode (default)
--verbose             # Detailed output
```

### Examples

```powershell
Build.bat                          # Build Release
Build.bat build Debug              # Build Debug
Build.bat run Debug --verbose      # Build Debug, run with verbose output
Build.bat rebuild Release          # Clean and rebuild Release
```

---

## Documentation

### 📖 Core Documentation

| Document | Description |
|----------|-------------|
| **[BUILD.md](docs/BUILD.md)** | Complete build guide with troubleshooting |
| **[ARCHITECTURE.md](docs/ARCHITECTURE.md)** | System architecture and design patterns |
| **[SHADER_PIPELINE.md](docs/SHADER_PIPELINE.md)** | Shader compilation and development |
| **[PROFILING.md](docs/PROFILING.md)** | Performance profiling and debugging |
| **[ROADMAP.md](docs/ROADMAP.md)** | Development roadmap and milestones |

### 📋 Configuration & Runtime

| Document | Description |
|----------|-------------|
| **[RUNTIME_CONFIG.md](docs/RUNTIME_CONFIG.md)** | Engine configuration (engine_config.ini) |
| **[ENGINE_CONFIG.md](docs/ENGINE_CONFIG.md)** | Legacy config documentation |

### 🎓 Guides

| Guide | Description |
|-------|-------------|
| **[IBL_SETUP.md](docs/guides/IBL_SETUP.md)** | Image-Based Lighting guide and troubleshooting |

### 🤖 AI Development

| Document | Description |
|----------|-------------|
| **[agents.md](agents.md)** | AI agent instructions and coding standards |

### 📦 Project Management

| Document | Description |
|----------|-------------|
| **[TODO.md](TODO.md)** | Development task tracking |
| **[IMPLEMENTATION_STATUS.md](IMPLEMENTATION_STATUS.md)** | Current implementation status |

---

## Dependencies

All dependencies are managed via Git submodules in `external/`:

| Library | Purpose | Status |
|---------|---------|--------|
| **bgfx** | Rendering abstraction | ✅ Required |
| **GLFW** | Window/input | ✅ Required |
| **GLM** | Math library | ✅ Required |
| **Bullet3** | Physics | ✅ Required |
| **ImGui** | UI | ✅ Required |
| **ImGuizmo** | 3D gizmos | ✅ Required |
| **STB** | Image loading | ✅ Required |

Dependencies are initialized automatically by `Build.bat setup`.

## Runtime Configuration

The engine supports configurable runtime duration through `engine_config.ini`:

### Quick Usage

```bash
# Set engine to run for 15 seconds then auto-exit (useful for testing)
powershell -ExecutionPolicy Bypass -File tools\Set-EngineRuntime.ps1 -Seconds 15 -CopyToBuild

# Run the engine (will exit after 15 seconds)
Build.bat run

# Reset to infinite runtime (normal behavior)
powershell -ExecutionPolicy Bypass -File tools\Set-EngineRuntime.ps1 -Seconds 0 -CopyToBuild
```

### Manual Configuration

Edit `engine_config.ini`:

```ini
[Runtime]
MaxRuntimeSeconds = 10  # Run for 10 seconds then exit
# MaxRuntimeSeconds = 0   # Run indefinitely (default)
```

**Use Cases:**
- ✅ **AI Agent Testing** - Automated validation without manual intervention
- ✅ **Frame Capture** - Capture specific duration for analysis
- ✅ **Performance Profiling** - Consistent test durations
- ✅ **CI/CD** - Prevent hanging builds

See **[docs/ENGINE_CONFIG.md](docs/ENGINE_CONFIG.md)** for complete documentation.

### Manual CMake (Alternative)

---

## Runtime Configuration

The engine supports runtime configuration via `engine_config.ini`:

```ini
[Runtime]
MaxRuntimeSeconds = 15     # Auto-exit after 15 seconds (0 = infinite)

[Window]
Width = 1280
Height = 720
Title = Anito Engine

[Rendering]
VSync = true
MSAA = 4

[Profiling]
EnableCPUProfiling = true
EnableGPUProfiling = true
FrameCaptureInterval = 1.0
```

**Use Cases:**
- ✅ AI agent testing (automated validation)
- ✅ Frame capture for specific durations
- ✅ Performance profiling with consistent test windows
- ✅ CI/CD integration (prevent hanging builds)

See **[RUNTIME_CONFIG.md](docs/RUNTIME_CONFIG.md)** for complete documentation.

---

## Project Structure

```
PBR-RT/
├── src/                      # Engine source code
│   ├── Math/                 # Vector, Matrix, Quaternion
│   ├── Renderer/             # Rendering system (bgfx)
│   ├── GameObjects/          # GameObject & Component system
│   ├── Components/           # Transform, Camera, MeshRenderer, etc.
│   ├── Input/                # Input management
│   ├── Window/               # Window management (GLFW)
│   ├── Debug/                # Profiling & debugging tools
│   └── main.cpp              # Entry point
│
├── assets/                   # Runtime assets
│   ├── shaders/              # Shader source (.sc) and compiled (.bin)
│   └── environments/         # HDR environment maps
│
├── external/                 # Third-party dependencies (Git submodules)
│   ├── bgfx/                 # Rendering abstraction
│   ├── glfw/                 # Window management
│   ├── glm/                  # Math library
│   ├── bullet3/              # Physics engine
│   ├── imgui/                # UI framework
│   └── ImGuizmo/             # 3D gizmos
│
├── docs/                     # Documentation
│   ├── BUILD.md              # Build instructions
│   ├── ARCHITECTURE.md       # System architecture
│   ├── SHADER_PIPELINE.md    # Shader development
│   ├── PROFILING.md          # Performance profiling
│   ├── ROADMAP.md            # Development roadmap
│   ├── guides/               # Step-by-step guides
│   └── archive/              # Historical documentation
│
├── tools/                    # Build and utility scripts
├── Build.bat                 # Unified build system
├── CMakeLists.txt            # CMake configuration
├── engine_config.ini         # Runtime configuration
├── agents.md                 # AI agent instructions
└── README.md                 # This file
```

---

## Development Status

### ✅ Phase 1: Core Foundation (COMPLETE)
- Math library (Vector, Matrix)
- Rendering abstractions (bgfx wrappers)
- GameObject & Component system
- Window & Input management
- Build system & dependency management

### ✅ Phase 2: PBR Rendering (COMPLETE)
- Deferred rendering pipeline
- PBR material system (metallic/roughness)
- Image-Based Lighting (IBL)
- HDR environment maps
- Tone mapping & gamma correction
- Test scenes & interactive demos
- Comprehensive profiling system

### 🚧 Phase 3: Advanced Lighting (IN PROGRESS)
- Shadow mapping (cascaded shadow maps)
- Advanced BRDF (Cook-Torrance)
- Screen-space techniques (SSAO, SSR)

### 🔜 Phase 4+: See [ROADMAP.md](docs/ROADMAP.md)
- Post-processing & anti-aliasing
- Volumetric effects
- Terrain & environment systems
- Animation & characters
- Physics & vehicles

---

## Technical Highlights

**Rendering:**
- Deferred PBR pipeline
- Image-Based Lighting (IBL) with HDR environments
- Filmic tone mapping
- Cross-platform via bgfx (Vulkan, DirectX, Metal)

**Architecture:**
- Component-based GameObject system
- Modular subsystems
- Data-oriented design principles
- bgfx abstraction for rendering portability

**Profiling:**
- CPU performance profiler (hierarchical timing)
- GPU profiler (render pass timing)
- Memory profiler (allocation tracking)
- Frame capture system (PNG screenshots)
- Crash reporting (Windows minidumps)
- JSON export for AI feedback loops

**Build System:**
- Unified AAA-style build script (`Build.bat`)
- CMake-based configuration
- Automatic dependency management (Git submodules)
- Shader compilation pipeline

---

## Support & Contribution

**Project Lead:** Neil Patrick Del Gallego, Ph.D.  
**Institution:** De La Salle University - GAME Lab  
**Funding:** DOST-PCIEERD

**Industry Partners:**
- Cybercraft
- Megacat Studios
- VISON Technologies

**For issues, questions, or contributions:**
- Check documentation in `/docs`
- Review `agents.md` for AI-assisted development guidelines
- See `TODO.md` for current tasks

---

## License

**DOST-PCIEERD Project Anito**  
De La Salle University - GAME Lab  
© 2025-2027

---

**Next Steps:**
1. Implement GameObject/Component system
2. Create camera system with orbit controls
3. Set up deferred rendering pipeline
4. Add basic PBR lighting
5. Implement shadow mapping
6. Add GPU-based object picking
7. Integrate ImGuizmo for transform manipulation
