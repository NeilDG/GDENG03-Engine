# Anito Engine - Development Roadmap

**Project Anito - DOST-PCIEERD Initiative**  
**DLSU GAME Lab**  
**Project Period:** June 2025 – June 2027

---

## Vision & Goals

Build a modular, high-performance game engine for **AAA visual fidelity** on **PC** and **Android** platforms, supporting:
- Physically-Based Rendering (PBR)
- Dynamic lighting and shadows
- Large-scale outdoor environments
- Atmospheric effects
- Real-time performance for titles like *Amnesia* (horror atmosphere) and *SnowRunner* (detailed worlds, physics-driven vehicles)

---

## Phase 1: Foundation ✅ **COMPLETE**

**Timeline:** Q1 2024  
**Status:** ✅ Completed December 2024

### Completed Features

#### Core Systems
- ✅ Math library (Vector2D/3D/4D, Matrix4x4 with GLM backend)
- ✅ GameObject component system
- ✅ Transform hierarchy
- ✅ Basic scene management
- ✅ Engine loop with delta time

#### Rendering Foundation
- ✅ bgfx integration (cross-platform rendering)
- ✅ Window management (GLFW)
- ✅ Vertex/index buffer management
- ✅ Shader loading system
- ✅ Texture loading (2D, cubemaps, HDR)
- ✅ Material system

#### Build System
- ✅ CMake configuration
- ✅ Unified Build.bat script (AAA-style)
- ✅ Dependency management (Git submodules)
- ✅ Shader compilation pipeline

---

## Phase 2: Core Rendering ✅ **COMPLETE**

**Timeline:** Q4 2024  
**Status:** ✅ Completed April 2026

### Completed Features

#### PBR Rendering
- ✅ Deferred rendering pipeline
- ✅ G-Buffer implementation
- ✅ PBR material system (metallic/roughness workflow)
- ✅ Image-Based Lighting (IBL)
  - ✅ HDR environment loading
  - ✅ Cubemap generation
  - ✅ Irradiance map (diffuse IBL)
  - ✅ Prefilter map (specular IBL)
  - ✅ BRDF lookup texture
- ✅ Filmic tone mapping
- ✅ Gamma correction

#### Test Framework
- ✅ PBR test scenes (5 interactive scenes)
- ✅ Metallic/roughness parameter testing
- ✅ IBL toggle system
- ✅ Scene switching

#### Profiling & Debugging
- ✅ CPU performance profiler
- ✅ GPU performance profiler
- ✅ Memory profiler (light/heavy modes)
- ✅ Console logger (dual output)
- ✅ Frame capture system (PNG screenshots)
- ✅ Crash reporting (Windows minidumps)
- ✅ JSON export for AI feedback loops

#### Runtime Configuration
- ✅ INI-based config system
- ✅ Window settings
- ✅ Rendering settings
- ✅ Profiling control
- ✅ Debug visualization

---

## Phase 3: Advanced Lighting & Shadows 🚧 **IN PROGRESS**

**Timeline:** Q2-Q3 2026  
**Status:** 🚧 Planning/Early Development

### 3.1 Shadow Mapping

#### Directional Lights
- [ ] Cascaded Shadow Maps (CSM)
  - [ ] 2-4 cascades
  - [ ] Smooth cascade transitions
  - [ ] Optimal shadow distance
- [ ] PCF (Percentage-Closer Filtering)
- [ ] VSM (Variance Shadow Maps) for soft shadows
- [ ] Shadow bias tuning

#### Point Lights
- [ ] Omnidirectional shadow maps (cubemap shadows)
- [ ] Efficient shadow atlas
- [ ] Shadow LOD system

#### Spot Lights
- [ ] Single shadow map per light
- [ ] Cone-based culling

#### Optimizations
- [ ] Shadow map caching
- [ ] Dynamic shadow resolution
- [ ] Frustum culling for shadow casters

**Deliverable:** Realistic shadows with adjustable quality settings

---

### 3.2 Advanced BRDF

- [ ] Cook-Torrance microfacet model (improved specular)
- [ ] Multiple BRDF lobes (diffuse + specular + clearcoat)
- [ ] Anisotropic reflections
- [ ] Sheen layer (for fabric materials)
- [ ] Subsurface scattering (SSS) for skin/wax

**Reference:** Disney BRDF, Unreal Engine 4 BRDF

**Deliverable:** Production-quality material system matching AAA standards

---

### 3.3 Screen-Space Techniques

- [ ] SSAO (Screen-Space Ambient Occlusion)
  - [ ] HBAO+ or GTAO variant
  - [ ] Multi-scale sampling
- [ ] SSR (Screen-Space Reflections)
  - [ ] Hierarchical Z-buffer
  - [ ] Reflection fade at edges
- [ ] Contact Shadows (fine detail shadows)

**Deliverable:** Enhanced visual fidelity with screen-space effects

---

## Phase 4: Post-Processing & Anti-Aliasing 🔜 **PLANNED**

**Timeline:** Q4 2026  
**Status:** 🔜 Not Started

### 4.1 Post-Processing Stack

- [ ] Bloom
  - [ ] Dual-filter downsampling
  - [ ] Adjustable threshold and intensity
  - [ ] Lens flare (optional)
- [ ] Depth of Field (DoF)
  - [ ] Bokeh-based blur
  - [ ] Cinematic focus
- [ ] Motion Blur
  - [ ] Per-object motion vectors
  - [ ] Camera motion blur
- [ ] Color Grading
  - [ ] LUT-based color correction
  - [ ] Adjustable saturation/contrast/brightness
- [ ] Vignette, Chromatic Aberration, Film Grain

**Deliverable:** Cinematic post-processing pipeline

---

### 4.2 Anti-Aliasing

- [ ] TAA (Temporal Anti-Aliasing)
  - [ ] Jittered sampling
  - [ ] History buffer reprojection
  - [ ] Ghosting reduction
- [ ] FXAA (Fast Approximate Anti-Aliasing) - fallback
- [ ] SMAA (Subpixel Morphological Anti-Aliasing)

**Deliverable:** Clean, alias-free rendering

---

## Phase 5: Advanced Rendering Features 🔜 **PLANNED**

**Timeline:** Q1-Q2 2027  
**Status:** 🔜 Not Started

### 5.1 Volumetric Effects

- [ ] Volumetric Fog
  - [ ] Exponential height fog
  - [ ] Froxel-based volumetric lighting
- [ ] Volumetric Clouds (for outdoor scenes)
- [ ] God Rays / Light Shafts

**Deliverable:** Atmospheric outdoor environments

---

### 5.2 Transparent Rendering

- [ ] Order-Independent Transparency (OIT)
  - [ ] Weighted Blended OIT or Depth Peeling
- [ ] Refraction (for glass, water)
- [ ] Frosted glass effects

**Deliverable:** Realistic transparent materials

---

### 5.3 Decals & Projections

- [ ] Deferred decals
- [ ] Projector lights
- [ ] Bullet holes, blood, weathering

**Deliverable:** Environmental detail and dynamic surface modifications

---

## Phase 6: Terrain & Environment 🔜 **PLANNED**

**Timeline:** Q3 2027  
**Status:** 🔜 Not Started

### 6.1 Terrain System

- [ ] Heightmap-based terrain
- [ ] LOD (Level of Detail) system
- [ ] Terrain texturing (splatmap blending)
- [ ] Terrain holes (caves, overhangs)
- [ ] Procedural terrain generation

**Deliverable:** Large-scale outdoor environments

---

### 6.2 Vegetation

- [ ] GPU instancing for grass/foliage
- [ ] Wind animation
- [ ] LOD transitions (imposters for distant trees)
- [ ] SpeedTree integration (optional)

**Deliverable:** Dense, performant vegetation

---

### 6.3 Water

- [ ] Planar reflections
- [ ] Water caustics
- [ ] Wave simulation (FFT-based or Gerstner waves)
- [ ] Underwater rendering

**Deliverable:** Realistic water bodies

---

## Phase 7: Animation & Characters 🔜 **PLANNED**

**Timeline:** Q4 2027  
**Status:** 🔜 Not Started

### 7.1 Skeletal Animation

- [ ] Skeletal mesh rendering
- [ ] Animation playback
- [ ] Animation blending
- [ ] State machines
- [ ] IK (Inverse Kinematics)

**Deliverable:** Animated characters

---

### 7.2 Morph Targets & Blend Shapes

- [ ] Facial animation
- [ ] Lip-sync support

**Deliverable:** Expressive character faces

---

## Phase 8: Physics & Vehicles 🔜 **PLANNED**

**Timeline:** Q1 2028  
**Status:** 🔜 Not Started

### 8.1 Advanced Physics

- [ ] Character controller (capsule-based)
- [ ] Ragdoll physics
- [ ] Cloth simulation
- [ ] Soft body physics

**Deliverable:** Realistic character movement and physics

---

### 8.2 Vehicle Physics

- [ ] Wheel colliders
- [ ] Suspension system
- [ ] Engine torque curves
- [ ] Tire friction model
- [ ] Damage system

**Reference:** SnowRunner, BeamNG.drive

**Deliverable:** Physics-driven vehicle simulation

---

## Phase 9: Audio System 🔜 **PLANNED**

**Timeline:** Q2 2028  
**Status:** 🔜 Not Started

- [ ] 3D spatial audio
- [ ] Reverb zones
- [ ] Audio occlusion
- [ ] Music system (layers, crossfade)
- [ ] FMOD or Wwise integration

**Deliverable:** Immersive audio experience

---

## Phase 10: Scripting & Tools 🔜 **PLANNED**

**Timeline:** Q3 2028  
**Status:** 🔜 Not Started

### 10.1 Scripting

- [ ] Lua integration
- [ ] C++ ↔ Lua binding (LuaBridge or Sol2)
- [ ] Hot-reloading scripts
- [ ] Visual scripting (optional)

**Deliverable:** Data-driven gameplay

---

### 10.2 Editor Tools

- [ ] Scene editor
- [ ] Material editor
- [ ] Particle editor
- [ ] Animation editor
- [ ] Terrain editor

**Deliverable:** Full engine editor suite

---

## Phase 11: Optimization & Platform Support 🔜 **PLANNED**

**Timeline:** Q4 2028  
**Status:** 🔜 Not Started

### 11.1 Performance Optimization

- [ ] Occlusion culling
- [ ] GPU-driven rendering
- [ ] Multi-threaded rendering
- [ ] Job system for CPU parallelism
- [ ] Memory optimizations

**Deliverable:** 60 FPS on target hardware

---

### 11.2 Android Port

- [ ] Touch input
- [ ] Mobile-optimized shaders
- [ ] Reduced memory footprint
- [ ] Vulkan renderer (Android)
- [ ] Device profiling (low/mid/high-end)

**Deliverable:** Fully functional Android build

---

## Ongoing: AI-Assisted Development

Throughout all phases:

- ✅ AI agent system (`agents.md`)
- ✅ Profiling feedback loops
- ✅ Automated testing
- ✅ Documentation generation
- ✅ Code quality analysis

**Goal:** Accelerate development with AI collaboration

---

## Success Metrics

| Metric | Target | Status |
|--------|--------|--------|
| **Frame Rate (PC)** | 60 FPS @ 1080p | ✅ Achieved |
| **Frame Rate (Android)** | 30 FPS @ 1080p | 🔜 Pending |
| **PBR Materials** | Production-quality | ✅ Achieved |
| **Lighting** | Real-time dynamic | 🚧 Partial |
| **Shadows** | High-quality soft shadows | 🔜 Planned |
| **Post-Processing** | Full AAA stack | 🔜 Planned |
| **Terrain** | 16km² open world | 🔜 Planned |
| **Physics** | SnowRunner-quality vehicles | 🔜 Planned |

---

## Dependencies & Research

### Key References
- **Physically Based Rendering Book** (pbr-book.org)
- **Real-Time Rendering, 4th Edition**
- **GPU Gems / GPU Pro series**
- **Unreal Engine source code** (for architecture patterns)
- **bgfx examples** (for implementation patterns)
- **Filament (Google)** (for PBR reference)

### External Libraries
- bgfx (rendering)
- Bullet3 (physics)
- ImGui/ImGuizmo (UI)
- Assimp (asset loading, planned)
- FMOD/Wwise (audio, planned)
- LuaJIT (scripting, planned)

---

## Team & Collaboration

- **Principal Investigator:** Neil Patrick Del Gallego, Ph.D.
- **AI Agents:** Architect, Renderer, Physics Engineer, Shader Specialist, etc.
- **Industry Partners:** Cybercraft, Megacat Studios, VISON Technologies
- **Students:** DLSU interns and researchers

---

## Conclusion

This roadmap reflects a **2-year development plan** to build a production-quality game engine. Priorities are:

1. **Visual Fidelity:** AAA-quality PBR rendering ✅
2. **Performance:** 60 FPS on target platforms 🚧
3. **Scalability:** PC + Android support 🔜
4. **Usability:** Full editor tools 🔜

**Next Milestone:** Phase 3 - Shadows and Advanced Lighting (Q2-Q3 2026)
