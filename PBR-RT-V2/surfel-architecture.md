# Surfel GI Prototype Architecture (Step 1)

## Scope
This document establishes the Step 1 architecture for a static surfel GI prototype in the existing Sponza-based `PBR-RT-V2` sample. The goal is to define subsystem boundaries and call flow without introducing full GI logic yet. Surfel-specific implementation files are organized under `src/SurfelRendering/`.

## Subsystems

### 1. Surfel (core data contract)
- Responsibility: Represent one persistent lighting sample anchored on scene geometry.
- Core fields for later steps:
  - position
  - normal
  - radius
  - albedo
  - irradiance
- Notes:
  - Keep compact and POD-like where possible.
  - Serves as the shared contract across bake, spatial indexing, gather, and debug view.

### 2. SurfelSceneBuilder (static bake boundary)
- Responsibility: Build surfel data from loaded Sponza scene geometry.
- Inputs:
  - Scene/model data from the app runtime (GLTF model and transforms).
  - Surfel placement configuration.
- Outputs:
  - Flat surfel array (`std::vector<Surfel>`).
- Step 1 behavior:
  - API scaffold only; no full sampling implementation.

### 3. SurfelSpatialGrid (neighbor lookup boundary)
- Responsibility: Accelerate surfel neighborhood queries during shading/gather.
- Inputs:
  - Surfel array.
  - Grid sizing config.
- Outputs:
  - Query result list for nearby surfel indices.
- Step 1 behavior:
  - API scaffold for build/query; no optimized data population yet.

### 4. SurfelGatherPass (indirect accumulation boundary)
- Responsibility: Compute surfel-driven indirect contribution for a shading point.
- Inputs:
  - Shading position/normal.
  - Nearby surfels from `SurfelSpatialGrid`.
- Outputs:
  - Indirect radiance/irradiance contribution.
- Step 1 behavior:
  - API scaffold only.

### 5. SurfelDebugView (visual diagnostics boundary)
- Responsibility: Expose toggles/modes to inspect surfel placement and density.
- Debug modes planned:
  - Disabled
  - Points
  - Normals
  - Radius/Density emphasis
- Step 1 behavior:
  - State and mode interface only.

### 6. NightSceneLightingPreset (scene lighting baseline)
- Responsibility: Provide a controlled night-time baseline for Sponza.
- Planned config:
  - Dark ambient baseline
  - Static point-light rig (minimum 5 lights)
  - Uniformly randomized RGB light colors with shared intensity/radius constraints
- Step 1 behavior:
  - Preset schema and setup hook only.

## Integration Points

### AnitoApplication
- Owns lifecycle coordination for surfel subsystems.
- High-level flow:
  1. Initialize renderer and load Sponza.
  2. Initialize/apply night preset.
  3. Build surfel set (once, static prototype).
  4. Build spatial grid from surfels.
  5. Configure renderer with surfel debug/gather dependencies.
- Runtime update:
  - Forward debug toggles from input to renderer/surfel debug view.

### Renderer
- Accepts surfel resources and debug mode state from the app layer.
- Provides stable integration hooks for:
  - surfel dataset binding
  - spatial query usage in future gather path
  - debug render mode selection
- Step 1 expectation:
  - Interface-only wiring; no final GI shading path yet.

### InputSystem
- Owns user-facing surfel debug toggles.
- Planned actions:
  - Toggle surfel debug enabled/disabled.
  - Cycle debug visualization mode.
- Step 1 expectation:
  - Input state contract only.

## Frame-Level Flow (target path)
1. App updates camera + input.
2. Input updates surfel debug state.
3. Renderer draws Sponza direct lighting baseline.
4. (Future) Renderer runs surfel gather path using spatial queries.
5. (Future) Renderer overlays surfel debug view when enabled.

## Ownership Summary
- `AnitoApplication`: orchestration and subsystem lifecycle.
- `Renderer`: rendering integration boundary.
- `InputSystem`: debug intent/state provider.
- `src/SurfelRendering/Surfel*` and `src/SurfelRendering/NightSceneLightingPreset*`: surfel data, build, lookup, gather, and visualization boundaries.

## Non-Goals for Step 1
- Full surfel generation from triangles.
- Production spatial hashing/tuning.
- Final GI shading integration and quality tuning.
- Performance optimization.
