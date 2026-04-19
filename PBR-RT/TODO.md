# Anito Engine - Development TODO List

## Immediate Priorities (This Week)

### 🔥 Critical Path: Get Visual Output
- [ ] **Set up bgfx shader compilation**
  - [ ] Install shaderc tool from bgfx
  - [ ] Create shader build pipeline
  - [ ] Set up varying.def.sc for vertex attributes

- [ ] **Create Basic Shaders**
  - [ ] Simple vertex shader (transform vertices)
  - [ ] Simple fragment shader (solid color or basic lighting)
  - [ ] Test shader loading with AnitoRenderer::createProgram()

- [ ] **Procedural Geometry**
  - [ ] Create cube mesh generator (8 vertices, 36 indices)
  - [ ] Create sphere mesh generator (UV sphere)
  - [ ] Create plane mesh generator
  - [ ] Wrap in AnitoVertexBuffer/AnitoIndexBuffer

- [ ] **First Render Test**
  - [ ] Create test cube GameObject
  - [ ] Add MeshRenderer component
  - [ ] Implement MeshRenderer::render() with actual draw calls
  - [ ] See rotating cube on screen!

---

## Phase 3A: Core Rendering (Week 1-2)

### Rendering Pipeline
- [ ] **Complete AnitoShader**
  - [ ] Shader compilation from .sc files
  - [ ] Uniform management
  - [ ] Shader program caching

- [ ] **Complete AnitoTexture**
  - [ ] Image loading (STB image or similar)
  - [ ] Texture creation and management
  - [ ] Sampler state configuration
  - [ ] Default white texture

- [ ] **Complete AnitoMaterial**
  - [ ] PBR parameter storage (albedo, metallic, roughness, etc.)
  - [ ] Texture slot management
  - [ ] Uniform binding
  - [ ] Material instances

- [ ] **Camera Controller**
  - [ ] Orbit camera (mouse drag to rotate)
  - [ ] Zoom with mouse wheel
  - [ ] Pan with middle mouse button
  - [ ] WASD free-fly mode

---

## Phase 3B: Deferred Rendering (Week 2-3)

### G-Buffer Implementation
- [ ] **Complete AnitoGBuffer**
  - [ ] Create render target textures (RGBA16F, RGB10A2, etc.)
  - [ ] Create framebuffer with multiple render targets
  - [ ] Implement bind() and resize()
  - [ ] Debug visualization of G-Buffer contents

- [ ] **G-Buffer Write Shaders**
  - [ ] Vertex shader (world space position/normal)
  - [ ] Fragment shader (pack material data to G-Buffer)
  - [ ] Support for PBR textures

- [ ] **Complete AnitoDeferredRenderer**
  - [ ] Geometry pass implementation
  - [ ] Lighting pass (fullscreen quad)
  - [ ] Forward pass for transparencies
  - [ ] Proper view management for each pass

---

## Phase 3C: Lighting System (Week 3-4)

### PBR Lighting
- [ ] **Lighting Shaders**
  - [ ] PBR BRDF implementation (Cook-Torrance)
  - [ ] Directional light support
  - [ ] Point light support
  - [ ] Spot light support

- [ ] **Light Management**
  - [ ] Light uniform buffer
  - [ ] Light culling (for deferred)
  - [ ] Light volume rendering
  - [ ] Debug light visualization

### Shadow Mapping
- [ ] **Complete AnitoShadowMap**
  - [ ] Create shadow framebuffer
  - [ ] Depth texture creation
  - [ ] Resize handling

- [ ] **Shadow Rendering**
  - [ ] Shadow map generation pass
  - [ ] Shadow projection matrix calculation
  - [ ] PCF filtering
  - [ ] Shadow bias configuration

- [ ] **Cascaded Shadows** (for directional lights)
  - [ ] Cascade split calculation
  - [ ] Multiple shadow maps
  - [ ] Cascade selection in shader
  - [ ] Debug cascade visualization

---

## Phase 4: Object Picking (Week 4-5)

### GPU Picking
- [ ] **ID Render Pass**
  - [ ] Create ID render target (R32UI)
  - [ ] Render objects with unique IDs
  - [ ] Read pixel at mouse position
  - [ ] Map ID back to GameObject

- [ ] **CPU Ray Casting**
  - [ ] Screen to world ray calculation
  - [ ] AABB intersection tests
  - [ ] Triangle intersection (for precise picking)
  - [ ] Gizmo interaction

### Selection System
- [ ] **Visual Feedback**
  - [ ] Outline rendering (stencil or post-process)
  - [ ] Highlight color
  - [ ] Bounding box display
  - [ ] Selection persistence

---

## Phase 5: Editor Tools (Week 5-6)

### ImGui Integration
- [ ] **Set up ImGui**
  - [ ] Initialize with bgfx backend
  - [ ] Configure for GLFW input
  - [ ] Set up docking layout

- [ ] **Core Panels**
  - [ ] Scene Hierarchy
    - [ ] Tree view of GameObjects
    - [ ] Drag-drop parenting (when hierarchy implemented)
    - [ ] Right-click context menu
  - [ ] Properties Inspector
    - [ ] Display selected object properties
    - [ ] Edit transform (position, rotation, scale)
    - [ ] Component list
    - [ ] Add/remove components
  - [ ] Viewport Window
    - [ ] Render scene in ImGui window
    - [ ] Handle viewport resize
    - [ ] Gizmo overlay
  - [ ] Console/Log
    - [ ] Capture std::cout/cerr
    - [ ] Color-coded messages
    - [ ] Clear and filter buttons

### ImGuizmo Integration
- [ ] **Transform Gizmo**
  - [ ] Initialize ImGuizmo
  - [ ] Gizmo mode switching (translate/rotate/scale)
  - [ ] Local vs world space
  - [ ] Snap settings
  - [ ] Apply transformations to selected object

---

## Phase 6: Advanced Features (Week 6+)

### Scene Management
- [ ] Scene loading/saving
- [ ] Multiple scenes
- [ ] Scene switching
- [ ] Prefab system

### Asset Pipeline
- [ ] Model loading (Assimp or custom)
- [ ] glTF 2.0 support (PBR standard)
- [ ] FBX support
- [ ] Asset browser panel
- [ ] Material editor

### Physics Integration
- [ ] Initialize Bullet Physics
- [ ] Rigidbody component
- [ ] Collider components (box, sphere, capsule, mesh)
- [ ] Physics debug rendering
- [ ] Physics material properties

### Post-Processing
- [ ] Tone mapping (ACES, Reinhard, etc.)
- [ ] Bloom
- [ ] SSAO (Screen Space Ambient Occlusion)
- [ ] Anti-aliasing (FXAA, TAA)
- [ ] Color grading
- [ ] Exposure control

### Performance Optimization
- [ ] Frustum culling
- [ ] Occlusion culling
- [ ] Level of detail (LOD)
- [ ] Texture streaming
- [ ] Batch rendering
- [ ] Instance rendering

---

## Quality of Life Improvements

### Developer Tools
- [ ] Performance profiler
- [ ] Memory tracker
- [ ] Shader hot-reload
- [ ] Asset hot-reload
- [ ] Build configurations (Debug, Release, Profile)

### Documentation
- [ ] Code comments for complex algorithms
- [ ] API documentation (Doxygen)
- [ ] Tutorial projects
- [ ] Example scenes

### Testing
- [ ] Unit tests for math library
- [ ] Integration tests for subsystems
- [ ] Render comparison tests
- [ ] Performance benchmarks

---

## Android Port (Later)

### Mobile Optimization
- [ ] Touch input support
- [ ] Mobile rendering optimizations
- [ ] Texture compression (ASTC)
- [ ] Lower-quality settings
- [ ] Battery optimization
- [ ] Device compatibility testing

---

## Research & Publications

### Benchmarking
- [ ] Performance comparison (vs Unity, Unreal, custom engines)
- [ ] API overhead measurement (bgfx vs raw Vulkan)
- [ ] Mobile performance profiling
- [ ] Large scene stress tests

### Documentation for Papers
- [ ] Architecture diagrams
- [ ] Performance charts
- [ ] Feature comparison tables
- [ ] Case studies

---

## Community & Collaboration

### Code Quality
- [ ] Code review process
- [ ] Coding standards enforcement
- [ ] Git workflow documentation
- [ ] Contribution guidelines

### Partnerships
- [ ] Cybercraft integration
- [ ] Megacat Studios feedback
- [ ] VISON Technologies collaboration
- [ ] Student projects

---

## Progress Tracking

### Milestones
- **M1**: First rendered object (cube/sphere)
- **M2**: PBR lighting working
- **M3**: Shadow mapping functional
- **M4**: Object picking operational
- **M5**: ImGui editor usable
- **M6**: Demo scene complete
- **M7**: Android build working
- **M8**: Publication submitted

### Current Status
- ✅ Architecture complete
- ✅ Build system ready
- ⚠️ Rendering in progress (M1 next)
- 📋 Lighting planned
- 📋 Shadows planned
- 📋 Picking planned
- 📋 Editor planned

---

## Notes

- Prioritize getting visual output ASAP (motivation boost!)
- Document decisions in IMPLEMENTATION_STATUS.md
- Update this TODO as tasks complete
- Don't over-engineer early - iterate!
- Focus on AAA quality, not feature quantity
- Keep Android compatibility in mind
- Test frequently on different hardware

---

**Remember:** The goal is a production-ready AAA engine for PC and Android, supporting Philippine gaming industry. Quality over speed!

---

*Updated: December 2024*  
*Anito Engine Development Team*
