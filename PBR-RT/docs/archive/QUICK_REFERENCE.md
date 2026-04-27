# Anito Engine - Quick Reference Card

## Running the Engine

```bash
# From project root
.\out\build\x64-debug\bin\AnitoEngine.exe

# Or press F5 in Visual Studio
```

## File Structure

```
GDENG03-Engine-PBR-RT/
├── src/
│   ├── Math/               # Vector/Matrix classes
│   ├── Renderer/           # bgfx rendering system
│   │   ├── Deferred/       # G-Buffer & deferred pipeline
│   │   └── Lighting/       # Lights & shadows
│   ├── GameObjects/        # GameObject & Component base
│   ├── Components/         # Transform, Camera, MeshRenderer
│   ├── Window/             # GLFW window management
│   ├── Input/              # Keyboard/mouse input
│   └── AnitoEngine.cpp     # Main engine class
├── external/               # Dependencies (git-ignored)
├── assets/                 # Shaders, models, textures
└── out/build/x64-debug/    # Build output
    └── bin/
        └── AnitoEngine.exe
```

## Key Classes

### Singleton Managers
```cpp
AnitoRenderer::getInstance()           // Rendering
AnitoGameObjectManager::getInstance()  // Scene objects
AnitoInputManager::getInstance()       // Input
```

### Creating Objects
```cpp
auto* obj = AnitoGameObjectManager::getInstance()->createObject(
    "MyObject", AnitoGameObject::PrimitiveType::Cube);

obj->setPosition(0, 0, -5);
obj->setRotation(0, 45, 0);
obj->setScale(1, 1, 1);
```

### Input Handling
```cpp
if (AnitoInputManager::getInstance()->isKeyDown(GLFW_KEY_W)) {
    // Move forward
}
```

## Build Commands

```bash
# Clean build
Remove-Item -Recurse -Force out

# Build in Visual Studio
Ctrl+Shift+B

# Run
F5 (debug) or Ctrl+F5 (release)
```

## Documentation Files

- `README.md` - Project overview
- `BUILD_SUCCESS.md` - ✅ Build completion status
- `QUICKSTART.md` - Developer onboarding
- `BUILDING.md` - Detailed build instructions
- `TODO.md` - Development roadmap
- `IMPLEMENTATION_STATUS.md` - Technical details
- `agents.md` - AI agent system

## What Works Now

✅ Window creation & management  
✅ bgfx rendering initialization  
✅ Input handling (keyboard, mouse)  
✅ GameObject/Component system  
✅ Transform, Camera components  
✅ Math library (Vector, Matrix)  

## What's Next

🔨 Shader compilation (bgfx shaderc)  
🔨 Basic PBR shaders  
🔨 Procedural geometry (cube, sphere)  
🔨 Actual mesh rendering  
🔨 Deferred lighting pass  

See TODO.md for full roadmap!

---

**Engine Status**: ✅ READY TO RUN
**Next Milestone**: Phase 3A - Visual Output (2-4 hours)
