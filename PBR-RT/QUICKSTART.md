# Anito Engine - Quick Start Guide

## For New Developers

Welcome to the Anito Engine codebase! This guide will help you get up and running quickly.

---

## Prerequisites

- **Windows 10/11** (macOS/Linux support planned)
- **Visual Studio 2022** or newer with C++20 support
- **CMake 3.10+**
- **Git**
- **PowerShell** (for setup script)

---

## Setup (5 minutes)

### Step 1: Clone the Repository
```bash
git clone <your-repo-url>
cd GDENG03-Engine-PBR-RT
```

### Step 2: Get Dependencies
Run the automated setup script:
```powershell
.\setup_dependencies.ps1
```

This will clone:
- bgfx (with bx/bimg)
- GLFW
- GLM
- Bullet Physics
- ImGui
- ImGuizmo

**Note:** This may take 5-10 minutes depending on your internet speed.

### Step 3: Configure CMake
```bash
cmake --preset=default
```

Or open the folder in Visual Studio - it will auto-configure.

### Step 4: Build
```bash
cmake --build out/build/default
```

Or build in Visual Studio (Ctrl+Shift+B).

### Step 5: Run
```bash
.\out\build\default\bin\AnitoEngine.exe
```

You should see a window with the Anito Engine title and a dark gray background.

---

## Project Structure

```
GDENG03-Engine-PBR-RT/
│
├── src/
│   ├── Math/                 # Vector and matrix classes
│   ├── Renderer/             # bgfx rendering wrappers
│   │   ├── Deferred/         # Deferred rendering pipeline
│   │   └── Lighting/         # Lights and shadows
│   ├── GameObjects/          # GameObject and Component system
│   ├── Components/           # Transform, Camera, MeshRenderer
│   ├── Window/               # GLFW window management
│   ├── Input/                # Keyboard/mouse input
│   ├── AnitoEngine.h/cpp     # Main engine class
│   └── main.cpp              # Entry point
│
├── external/                 # Third-party libraries (git-ignored)
├── assets/                   # Shaders, models, textures (TBD)
├── CMakeLists.txt            # Build configuration
├── README.md                 # Overview
├── IMPLEMENTATION_STATUS.md  # Detailed status report
└── agents.md                 # AI agent system documentation
```

---

## Adding a New Feature

### Example: Creating a Rotating Cube

1. **In `AnitoEngine::initialize()`**, after subsystems init:
```cpp
// Create a test cube
auto* cube = AnitoGameObjectManager::getInstance()->createObject(
    "TestCube", 
    AnitoGameObject::PrimitiveType::Cube
);
cube->setPosition(0, 0, -5);
```

2. **In `AnitoEngine::update()`**, add rotation:
```cpp
// Rotate cube
auto* cube = AnitoGameObjectManager::getInstance()->findObjectByName("TestCube");
if (cube) {
    auto rot = cube->getRotation();
    cube->setRotation(rot.x(), rot.y() + deltaTime, rot.z());
}
```

**Note:** This won't render anything yet because we haven't implemented mesh geometry or shaders. See IMPLEMENTATION_STATUS.md for what's complete.

---

## Current Capabilities

### ✅ What Works
- Window creation and input handling
- GameObject/Component system
- Transform management
- Camera setup (view/projection matrices)
- Basic rendering loop

### ⚠️ What's Stubbed
- Actual mesh rendering (no shaders yet)
- Deferred pipeline (G-Buffer not implemented)
- Lighting (equations not implemented)
- Shadows (stub only)
- Material system (placeholder)

### 📋 What's Planned
- Shader compilation (bgfx shaderc)
- PBR materials
- Real-time shadows
- GPU object picking
- ImGui editor UI
- ImGuizmo transform tools

---

## Key Classes

### Singleton Managers
```cpp
AnitoRenderer::getInstance()              // Rendering
AnitoGameObjectManager::getInstance()     // Scene objects
AnitoInputManager::getInstance()          // Input
```

### Creating Objects
```cpp
auto* obj = new AnitoGameObject("MyObject");
obj->setPosition(x, y, z);
obj->setRotation(rx, ry, rz);
obj->setScale(sx, sy, sz);

auto* camera = new AnitoCamera();
obj->attachComponent(camera);

AnitoGameObjectManager::getInstance()->addObject(obj);
```

### Input
```cpp
if (AnitoInputManager::getInstance()->isKeyDown(GLFW_KEY_W)) {
    // Move forward
}

if (AnitoInputManager::getInstance()->isMouseButtonDown(
    AnitoInputManager::MouseButton::Left)) {
    // Handle click
}
```

---

## Development Workflow

1. **Read `agents.md`** - Understand the AI agent system and your role
2. **Check `IMPLEMENTATION_STATUS.md`** - See what's done and what's next
3. **Make changes** - Follow coding patterns from reference implementation
4. **Build** - `cmake --build out/build/default`
5. **Test** - Run the engine
6. **Update docs** - Keep IMPLEMENTATION_STATUS.md current

---

## Coding Standards

Following **Anito Engine** conventions:

1. **Naming**: `Anito` prefix for all classes (e.g., `AnitoRenderer`)
2. **Style**: 
   - PascalCase for classes
   - camelCase for methods
   - m_ prefix for member variables
   - s_ prefix for static members
3. **C++ Standard**: C++20
4. **Comments**: Only when necessary (code should be self-documenting)
5. **Architecture**: Follow reference implementation patterns (singleton managers, component-based objects)

---

## Debugging Tips

### No window appears
- Check console for error messages
- Verify GLFW initialized correctly
- Check native window handle extraction

### Build errors
- Ensure dependencies are in `external/` directory
- Re-run `setup_dependencies.ps1`
- Delete `out/` and reconfigure CMake

### Performance issues
- bgfx debug layer is enabled by default
- Use Release build for performance testing
- Check bgfx stats with F1 key (when stats overlay implemented)

---

## Next Steps for Development

Based on IMPLEMENTATION_STATUS.md, the immediate next steps are:

### Phase 3A: Shader System & Rendering
1. Set up bgfx shader compilation pipeline
2. Create basic vertex/fragment shaders
3. Implement G-Buffer rendering
4. Create procedural cube/sphere geometry
5. Render first visible object!

### Phase 3B: Lighting
6. Implement PBR lighting equations
7. Add directional light
8. Implement shadow mapping

See IMPLEMENTATION_STATUS.md "Next Implementation Steps" for details.

---

## Resources

- **bgfx Documentation**: https://bkaradzic.github.io/bgfx/
- **bgfx Examples**: https://bkaradzic.github.io/bgfx/examples.html
- **PBR Theory**: https://learnopengl.com/PBR/Theory
- **Deferred Rendering**: https://learnopengl.com/Advanced-Lighting/Deferred-Shading
- **glTF PBR**: https://github.khronos.org/glTF-Tutorials/PBR/
- **Real-Time Rendering**: https://www.realtimerendering.com/

---

## Getting Help

- Check **agents.md** for AI agent guidance
- Review **reference implementation** at `X:\GithubProjects\GDENG03-Engine\Part 12 - IMGuizmo Integration`
- Read bgfx examples for rendering patterns
- Consult Project Anito documentation

---

## Contributing

When making changes:
1. Follow the architecture defined in `agents.md`
2. Update `IMPLEMENTATION_STATUS.md` with progress
3. Add comments for complex algorithms
4. Test on Windows first (Android later)
5. Commit with clear messages

---

**Happy Coding!**

*Building AAA game engine technology for the Philippine gaming industry.*

*- Anito Engine Development Team*  
*DLSU GAME Lab • Project Anito (DOST-PCIEERD)*
