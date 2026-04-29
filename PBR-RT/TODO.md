# Anito Engine - Development TODO List

**Last Updated:** April 29, 2026  
**Current Status:** Phase 3 Complete - Deferred Rendering with PBR + IBL

---

## ✅ **COMPLETED: Phase 3 - Deferred Rendering & PBR**

All Phase 3 objectives have been successfully completed:

### ✅ Phase 3A: Core Rendering
- ✅ **bgfx shader compilation** - Integrated into Build.bat
- ✅ **Basic shaders** - Complete PBR shader suite
- ✅ **Procedural geometry** - Sphere, cube generators
- ✅ **AnitoShader** - Full shader compilation and caching
- ✅ **AnitoTexture** - HDR, cubemap, 2D texture support
- ✅ **AnitoMaterial** - PBR material system
- ✅ **Camera controller** - FPS camera with left-click rotation

### ✅ Phase 3B: Deferred Rendering
- ✅ **AnitoGBuffer** - Full G-Buffer implementation with MRT
- ✅ **G-Buffer shaders** - Geometry pass with material encoding
- ✅ **AnitoDeferredRenderer** - Geometry + lighting pass system
- ✅ **Debug visualization** - G-Buffer debug view

### ✅ Phase 3C: PBR Lighting & IBL
- ✅ **Cook-Torrance BRDF** - Full PBR lighting implementation
- ✅ **Directional light** - Single directional light support
- ✅ **Image-Based Lighting** - HDR environment, irradiance, prefilter maps
- ✅ **Test scenes** - 5 interactive PBR test scenes
- ✅ **Profiling suite** - CPU, GPU, memory profiling
- ✅ **Benchmarking system** - Multi-resolution automated testing

**See:** `IMPLEMENTATION_STATUS.md`, `ROADMAP.md` for complete details

---

## 🚀 **NEXT: Phase 4 - Advanced Rendering Features**

Priority features for the next development phase:

### Shadow Mapping (High Priority)
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

### Point & Spot Lights
- [ ] **Point light support** - Omnidirectional shadow maps
- [ ] **Spot light support** - Cone-based shadow projection
- [ ] **Light management system** - Dynamic light addition/removal

---

## Phase 5: Scene Management & Editor Tools

### Object Picking
- [ ] **GPU Picking**
  - [ ] ID render pass with unique object IDs
  - [ ] Pixel readback at mouse position
  - [ ] Map ID to GameObject

- [ ] **Visual Feedback**
  - [ ] Outline rendering (stencil or post-process)
  - [ ] Highlight color
  - [ ] Bounding box display
  - [ ] Selection persistence

### ImGui Integration (Already Partially Implemented)
- [x] **ImGui initialized** - Basic setup complete
- [ ] **Enhanced UI Panels**
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

## Phase 6: Post-Processing & Visual Effects

### Post-Processing Pipeline
- [ ] **Post-process framework**
  - [ ] Render to intermediate buffer
  - [ ] Post-process pass system
  - [ ] Effect stacking
- [ ] Tone mapping (ACES, Reinhard, etc.)
- [ ] Bloom
- [ ] SSAO (Screen Space Ambient Occlusion)
- [ ] Anti-aliasing (FXAA, TAA)
- [ ] Color grading
- [ ] Exposure control

### Advanced Visual Effects
- [ ] **Screen-space effects**
  - [ ] Screen-space reflections (SSR)
  - [ ] Contact shadows
- [ ] **Volumetric effects**
  - [ ] Fog system
  - [ ] Volumetric lighting
  - [ ] Particle systems

---

## Phase 7: Asset Pipeline & Scene Management

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

---

## Phase 8: Physics Integration (Bullet Physics Already Integrated)

### Physics System Enhancement
- [x] **Bullet Physics** - Base integration complete
- [ ] **Enhanced components**
  - [ ] Rigidbody component with full API
  - [ ] Collider components (box, sphere, capsule, mesh)
  - [ ] Physics material properties
  - [ ] Constraint system
- [ ] **Debug tools**
  - [ ] Physics debug rendering
  - [ ] Collision visualization
  - [ ] Performance profiling

---

## Phase 9: Performance Optimization & Quality Improvements

### Rendering Optimizations
- [ ] Frustum culling
- [ ] Occlusion culling
- [ ] Level of detail (LOD)
- [ ] Texture streaming
- [ ] Batch rendering
- [ ] Instance rendering

### Developer Tools (Partially Complete)
- [x] **Performance profiler** - CPU, GPU, memory profiling complete
- [x] **Crash reporter** - Windows minidump system
- [ ] Shader hot-reload
- [ ] Asset hot-reload
- [ ] Build configurations enhancement

### Documentation
- [ ] Code comments for complex algorithms
- [ ] API documentation (Doxygen)
- [ ] Tutorial projects
- [ ] Example scenes

### Testing
- [x] **Profiling suite** - Complete with JSON/CSV export
- [x] **Automated benchmarks** - Multi-resolution testing system
- [ ] Unit tests for math library
- [ ] Integration tests for subsystems
- [ ] Render comparison tests

---

## Phase 10: Android Port

### Mobile Optimization
- [ ] Touch input support
- [ ] Mobile rendering optimizations
- [ ] Texture compression (ASTC)
- [ ] Lower-quality settings
- [ ] Battery optimization
- [ ] Device compatibility testing

---

## Phase 11: Research & Publications (DOST-PCIEERD Deliverables)

### Benchmarking (In Progress)
- [x] **Automated benchmark system** - Phase 3 Step 12-13 complete
- [x] **JSON/CSV data export** - AI-ready format
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

### Completed Milestones ✅
- **M1**: ✅ First rendered object (sphere with PBR)
- **M2**: ✅ PBR lighting working (Cook-Torrance BRDF)
- **M2.5**: ✅ IBL working (irradiance + prefilter)
- **M2.6**: ✅ Deferred rendering complete
- **M2.7**: ✅ Automated benchmarking system

### Current Phase
**Phase 3 Complete** - Deferred Rendering with PBR + IBL

### Next Milestones
- **M3**: Shadow mapping functional (Directional lights)
- **M4**: Object picking operational
- **M5**: ImGui editor enhanced
- **M6**: Demo scene complete
- **M7**: Android build working
- **M8**: Publication submitted

### Overall Status
- ✅ **Architecture** - Complete and stable
- ✅ **Build system** - Unified Build.bat system
- ✅ **Foundation rendering** - PBR + IBL + Deferred complete
- ✅ **Profiling suite** - CPU/GPU/Memory + Benchmarking
- 🚀 **Shadow mapping** - Next priority (Phase 4)
- 📋 **Object picking** - Planned (Phase 5)
- 📋 **Editor enhancement** - Planned (Phase 5)
- 📋 **Android port** - Planned (Phase 10)

**Last Updated:** April 29, 2026  
**For detailed status, see:** `IMPLEMENTATION_STATUS.md`, `ROADMAP.md`

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
