# Anito Engine - Architecture Overview

**Modular Game Engine Architecture**  
**DLSU GAME Lab - Project Anito**

---

## Table of Contents
1. [System Architecture](#system-architecture)
2. [Core Subsystems](#core-subsystems)
3. [Rendering Pipeline](#rendering-pipeline)
4. [Data Flow](#data-flow)
5. [Design Principles](#design-principles)

---

## System Architecture

### High-Level Overview

Anito Engine follows modern AAA game engine architecture principles:

```
┌─────────────────────────────────────────────────────────────┐
│                      Application Layer                       │
│  (Game Logic, Scene Management, Entity Systems)              │
└──────────────────────┬──────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                      Engine Core                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │   ECS    │  │ Resource │  │   Job    │  │  Event   │   │
│  │  System  │  │ Manager  │  │  System  │  │   Bus    │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
└──────────────────────┬──────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                  Subsystem Layer                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │Rendering │  │ Physics  │  │  Audio   │  │  Input   │   │
│  │  System  │  │  System  │  │  System  │  │  System  │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
└──────────────────────┬──────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                   Platform Layer                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │  Window  │  │   File   │  │ Network  │  │  Thread  │   │
│  │  (GLFW)  │  │  System  │  │   I/O    │  │ Primitives│  │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
└──────────────────────┬──────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                  Graphics API Layer                          │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              bgfx (Abstraction)                      │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐          │   │
│  │  │  Vulkan  │  │ DirectX  │  │  Metal   │          │   │
│  │  │          │  │  11/12   │  │          │          │   │
│  │  └──────────┘  └──────────┘  └──────────┘          │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### Module Dependencies

```
AnitoEngine (Core)
├── AnitoRenderer (Rendering)
│   ├── bgfx (Graphics API abstraction)
│   ├── AnitoShader
│   ├── AnitoTexture
│   ├── AnitoMaterial
│   ├── AnitoVertexBuffer
│   └── AnitoIndexBuffer
│
├── AnitoPhysics (Physics)
│   └── Bullet3
│
├── AnitoInput (Input)
│   └── GLFW
│
├── AnitoWindow (Windowing)
│   └── GLFW
│
├── AnitoMath (Mathematics)
│   └── GLM
│
└── AnitoUI (User Interface)
    ├── ImGui
    └── ImGuizmo
```

---

## Core Subsystems

### 1. Entity-Component-System (ECS)

**Status:** Partial implementation (component-based GameObjects)

```cpp
// Current Architecture (GameObject-based)
class AnitoGameObject {
    std::string name;
    std::unique_ptr<AnitoTransform> transform;
    std::vector<std::unique_ptr<AnitoComponent>> components;
    std::vector<std::unique_ptr<AnitoGameObject>> children;
};

// Components
class AnitoComponent {
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
};

// Examples:
// - AnitoTransform
// - AnitoMeshRenderer
// - AnitoCamera
// - AnitoLight (Directional, Point, Spot)
```

**Future:** Full ECS with data-oriented design for performance.

### 2. Rendering System

**Current:** Deferred PBR rendering with Image-Based Lighting (IBL)

```cpp
class AnitoRenderer {
public:
    // Core initialization
    void initialize(AnitoWindow* window);
    void shutdown();

    // Frame management
    void beginFrame();
    void endFrame();

    // Rendering
    void renderScene(AnitoScene* scene);
    void renderSkybox(AnitoTexture* cubemap);

    // IBL management
    void loadHDREnvironment(const std::string& path);
    void generateIBLMaps();

    // Deferred rendering (G-Buffer)
    void renderGeometryPass();
    void renderLightingPass();
    void renderPostProcessPass();
};
```

**Features:**
- ✅ Deferred rendering pipeline
- ✅ PBR materials (metallic/roughness workflow)
- ✅ Image-Based Lighting (IBL)
- ✅ HDR environment maps
- ✅ Tone mapping (Filmic)
- ✅ Shadow mapping (planned)

### 3. Resource Management

**Current:** Simple resource loading and caching

```cpp
class AnitoResourceManager {
public:
    // Textures
    std::shared_ptr<AnitoTexture> loadTexture(const std::string& path);

    // Shaders
    std::shared_ptr<AnitoShader> loadShader(const std::string& vsPath, 
                                            const std::string& fsPath);

    // Materials
    std::shared_ptr<AnitoMaterial> createMaterial(const std::string& name);

    // Meshes
    std::shared_ptr<AnitoMesh> loadMesh(const std::string& path);

private:
    std::unordered_map<std::string, std::shared_ptr<AnitoTexture>> m_textureCache;
    std::unordered_map<std::string, std::shared_ptr<AnitoShader>> m_shaderCache;
};
```

**Features:**
- ✅ Automatic resource caching
- ✅ Reference counting (std::shared_ptr)
- 🚧 Hot-reloading (planned)
- 🚧 Async loading (planned)

### 4. Physics System

**Status:** Integrated with Bullet Physics 3

```cpp
class AnitoPhysicsWorld {
public:
    void initialize();
    void update(float deltaTime);

    void addRigidBody(AnitoRigidBody* body);
    void removeRigidBody(AnitoRigidBody* body);

    bool raycast(const glm::vec3& origin, const glm::vec3& direction,
                 AnitoRaycastHit& hit);
};

// Components
class AnitoRigidBody : public AnitoComponent {
    btRigidBody* m_bulletBody;
    float mass;
    bool isKinematic;
};

class AnitoCollider : public AnitoComponent {
    btCollisionShape* m_shape;
    // Box, Sphere, Capsule, Mesh, etc.
};
```

**Features:**
- ✅ Rigid body dynamics
- ✅ Collision detection
- ✅ Raycasting
- 🚧 Character controller (planned)
- 🚧 Soft body physics (planned)

### 5. Input System

**Current:** GLFW-based input handling

```cpp
class AnitoInputManager {
public:
    // Keyboard
    bool isKeyDown(int key);
    bool isKeyPressed(int key);
    bool isKeyReleased(int key);

    // Mouse
    glm::vec2 getMousePosition();
    glm::vec2 getMouseDelta();
    bool isMouseButtonDown(int button);
    float getScrollDelta();

    // Internal
    void update();  // Call per frame
};
```

**Features:**
- ✅ Keyboard input
- ✅ Mouse input
- ✅ Scroll wheel
- 🚧 Gamepad support (planned)
- 🚧 Touch input (Android, planned)

---

## Rendering Pipeline

### Deferred PBR Pipeline

```
┌─────────────────────────────────────────────────────────────┐
│                    Geometry Pass (G-Buffer)                  │
│  Output: Position, Normal, Albedo, Metallic/Roughness/AO    │
└──────────────────────┬──────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                     Lighting Pass                            │
│  Input: G-Buffer + Light data                               │
│  Process: PBR BRDF + IBL + Shadow maps                      │
│  Output: HDR color                                           │
└──────────────────────┬──────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                    Skybox Pass                               │
│  Render: HDR environment to background                      │
└──────────────────────┬──────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                 Post-Processing Pass                         │
│  - Tone mapping (Filmic)                                     │
│  - Gamma correction                                          │
│  - Bloom (planned)                                           │
│  - Anti-aliasing (planned)                                   │
└──────────────────────┬──────────────────────────────────────┘
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                   UI/Debug Pass                              │
│  ImGui overlay, gizmos, debug visualization                  │
└──────────────────────┬──────────────────────────────────────┘
                       │
                    Present
```

### G-Buffer Layout

| Attachment | Format | Contents |
|------------|--------|----------|
| **GBuffer0** | RGBA16F | Position (RGB), Material ID (A) |
| **GBuffer1** | RGBA16F | Normal (RGB), Unused (A) |
| **GBuffer2** | RGBA8 | Albedo (RGB), AO (A) |
| **GBuffer3** | RGBA8 | Metallic (R), Roughness (G), Unused (B, A) |
| **Depth** | D24S8 | Depth/Stencil |

### IBL Integration

```cpp
// HDR Environment → Cubemap
AnitoTexture* hdriTexture = loadHDR("environment.hdr");
AnitoTexture* environmentCubemap = convertToCubemap(hdriTexture);

// Generate IBL maps
AnitoTexture* irradianceMap = generateIrradianceMap(environmentCubemap);   // Diffuse IBL
AnitoTexture* prefilterMap = generatePrefilterMap(environmentCubemap);     // Specular IBL (multiple mips)
AnitoTexture* brdfLUT = generateBRDF_LUT();                                 // Split-sum BRDF lookup

// Use in PBR shader
shader->setTexture("s_irradianceMap", irradianceMap, 0);
shader->setTexture("s_prefilterMap", prefilterMap, 1);
shader->setTexture("s_brdfLUT", brdfLUT, 2);
```

---

## Data Flow

### Frame Update Loop

```cpp
// In AnitoEngine::run()
while (!shouldClose) {
    // 1. Input polling
    inputManager->update();

    // 2. Physics simulation
    physicsWorld->update(deltaTime);

    // 3. Game logic update
    scene->update(deltaTime);

    // 4. Rendering
    renderer->beginFrame();
    renderer->renderScene(scene);
    renderer->endFrame();

    // 5. Profiling
    profiler->endFrame();
}
```

### Rendering Frame

```cpp
void AnitoRenderer::renderScene(AnitoScene* scene) {
    // 1. Geometry pass
    setView(GEOMETRY_VIEW);
    setFrameBuffer(gBuffer);
    for (auto& obj : scene->objects) {
        if (auto* meshRenderer = obj->getComponent<AnitoMeshRenderer>()) {
            meshRenderer->render();
        }
    }

    // 2. Skybox pass
    setView(SKYBOX_VIEW);
    renderSkybox(scene->environmentMap);

    // 3. Lighting pass
    setView(LIGHTING_VIEW);
    setFrameBuffer(hdrBuffer);
    renderFullscreenQuad(lightingShader);

    // 4. Post-processing
    setView(POSTPROCESS_VIEW);
    setFrameBuffer(nullptr);  // Back buffer
    renderFullscreenQuad(postProcessShader);

    // 5. UI
    setView(UI_VIEW);
    renderUI();
}
```

---

## Design Principles

### 1. Modularity

Each subsystem is self-contained with clear interfaces:

```cpp
// Subsystem interface
class ISubsystem {
public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update(float deltaTime) = 0;
};

// Engine manages subsystems
class AnitoEngine {
    std::vector<std::unique_ptr<ISubsystem>> m_subsystems;

    void addSubsystem(std::unique_ptr<ISubsystem> subsystem);
};
```

### 2. Data-Oriented Design

**Goal:** Cache-friendly, SIMD-friendly data layouts

**Example:**
```cpp
// Bad: Array of Structures (AoS)
struct Transform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};
std::vector<Transform> transforms;

// Good: Structure of Arrays (SoA)
struct TransformData {
    std::vector<glm::vec3> positions;
    std::vector<glm::quat> rotations;
    std::vector<glm::vec3> scales;
};
```

### 3. Separation of Concerns

- **Logic:** Game code, AI, gameplay
- **Presentation:** Rendering, audio, UI
- **Simulation:** Physics, animation, networking

### 4. Cross-Platform from Day One

- Use abstractions: bgfx (rendering), GLFW (window), Bullet (physics)
- Avoid platform-specific code in core systems
- Test on Windows and Android regularly

### 5. Performance First

- Profile early and often
- Minimize memory allocations in hot paths
- Use object pools for frequently created/destroyed objects
- Batch rendering calls

### 6. Extensibility

- Plugin architecture for custom components
- Scriptable gameplay via Lua (planned)
- Data-driven design (JSON/XML configs)

---

## Technology Stack

| Layer | Technology | Purpose |
|-------|------------|---------|
| **Rendering** | bgfx | Cross-platform graphics API abstraction |
| **Math** | GLM | Vector/matrix math, transforms |
| **Physics** | Bullet3 | Rigid body dynamics, collision detection |
| **Windowing** | GLFW | Window creation, input handling |
| **UI** | ImGui + ImGuizmo | Editor UI, debug tools, gizmos |
| **Image Loading** | STB Image | PNG, TGA, HDR texture loading |
| **Build** | CMake | Cross-platform build configuration |
| **Version Control** | Git | Source control, collaboration |

---

## Performance Targets

| Metric | Target | Notes |
|--------|--------|-------|
| **Frame Rate** | 60 FPS (16.67ms) | PC High-End |
| **Frame Rate** | 30 FPS (33.33ms) | Android Mid-Range |
| **CPU Frame Time** | < 10ms | Leaves headroom for spikes |
| **GPU Frame Time** | < 12ms | Depends on resolution |
| **Memory (PC)** | < 4GB | Including assets |
| **Memory (Android)** | < 2GB | Mobile constraints |
| **Startup Time** | < 5 seconds | From launch to playable |

---

## Future Roadmap

See [ROADMAP.md](ROADMAP.md) for detailed development plans.

**Key upcoming features:**
- 🚧 Shadow mapping (cascaded shadow maps for directional lights)
- 🚧 SSAO (screen-space ambient occlusion)
- 🚧 Bloom post-processing
- 🚧 TAA (temporal anti-aliasing)
- 🚧 Volumetric fog/lighting
- 🚧 Terrain system
- 🚧 Particle systems
- 🚧 Animation system (skeletal, blend shapes)
- 🚧 Audio system (3D spatial audio)
- 🚧 Scripting (Lua integration)

---

## Reference

For implementation details, see:
- **Component System:** `src/GameObjects/` and `src/Components/`
- **Rendering:** `src/Renderer/`
- **Math:** `src/Math/`
- **Physics:** `src/Physics/` (planned)

For architecture decisions and design patterns:
- `agents.md` - AI agent roles and responsibilities
- `docs/` - Additional technical documentation
