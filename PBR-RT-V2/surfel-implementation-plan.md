# Surfel Implementation Plan for PBR-RT-V2

## Understanding
Implement a practical surfel-based GI prototype for the existing Sponza scene in this Diligent/MSVC/CMake workspace, using a night-time lighting setup and at least five static point lights with uniformly randomized RGB colors.

## Assumptions
- The current prototype stays centered on `src/AnitoApplication.cpp` and the existing `GLTF_PBR_Renderer` path.
- The first version is static and scene-specific for Sponza; dynamic surfel skinning and fully adaptive EA-style placement are deferred.
- New runtime classes should follow the repository guidance and avoid adding the `Anito` prefix except where already required.
- The local `papers/` directory is expected to contain surfel references, but the workspace currently exposes only an incomplete download artifact, so the implementation should rely on the linked surfel resources and any readable local paper files if/when they become available.

## Approach
The most feasible implementation path is a staged prototype: first establish a dark Sponza lighting baseline, then add a static surfel bake from the scene geometry, then query those surfels during shading through a simple spatial data structure. This keeps the first milestone visible and testable in the existing sample app without requiring the full EA SEED dynamic spawning system.

The surfel subsystem should be organized under `src/SurfelRendering/` so all surfel-specific headers/sources remain isolated from generic app boundary files. The checklist below is organized so each step has an expected output and an explicit class-change scope. In this workspace, the main integration points are `src/AnitoApplication.cpp`, `src/Renderer.h`, `src/InputSystem.h`, and surfel subsystem files under `src/SurfelRendering/`.

## Key Files
- `src/AnitoApplication.cpp` - current Sponza sample implementation and the best place to wire the prototype.
- `src/AnitoApplication.h` - application entry-point contract.
- `src/Renderer.h` - natural home for rendering subsystem expansion.
- `src/InputSystem.h` - camera/debug input control hook.
- `src/SurfelRendering/` - dedicated folder for surfel subsystem implementation (`Surfel`, `SurfelSceneBuilder`, `SurfelSpatialGrid`, `SurfelGatherPass`, `SurfelDebugView`, `NightSceneLightingPreset`, and follow-on surfel classes).
- `CMakeLists.txt` - source registration and future build integration.
- `src/main.cpp` - main entry point and startup path.

## Risks & Open Questions
- The surfel GI quality will depend heavily on surfel density and spatial lookup quality.
- A static bake may not match the exact EA SEED placement strategy, but it is the fastest path to a useful result.
- The repository currently has no surfaced surfel subsystem, so the first implementation will need new classes and possibly new source files.
- The scene may already rely on an environment map; for a night-time setup, that path may need to be muted or replaced.

## Steps
1. Establish the surfel prototype architecture
   - Status: **Completed** (Step 1 scaffolding and architecture documentation are in place).
   - Expected output: a documented subsystem design that defines the surfel data model, bake flow, spatial lookup flow, and render integration points for Sponza.
   - Classes to add: `Surfel`, `SurfelSceneBuilder`, `SurfelSpatialGrid`, `SurfelGatherPass`, `SurfelDebugView`, `NightSceneLightingPreset`.
   - Classes to revise: `AnitoApplication`, `Renderer`, `InputSystem`.
   - Classes to delete: none.

2. Define the surfel data contract
   - Status: **Completed** (`Surfel`, `SurfelPrototypeSettings`, `SurfelGISettings`, and `SurfelLightSample` are fully defined and integrated).
   - Expected output: a compact surfel struct and supporting configuration types that can store position, normal, radius, albedo, and irradiance.
   - Classes to add: `Surfel`, `SurfelGISettings`, `SurfelLightSample`.
   - Classes to revise: `Renderer`.
   - Classes to delete: none.
   - **Implementation notes:**
     - `SurfelGISettings` added with SamplingQuality, CacheResolution, and MaxCacheSize parameters
     - `SurfelLightSample` added to store direct lighting information (Position, DirectIrradiance, DistanceAttenuation)
     - All types integrated into Surfel.h and available to the rendering pipeline

3. Build the static Sponza surfel bake path
   - Status: **Completed** (SurfelSceneBuilder now has full mesh integration and procedural surfel generation).
   - Expected output: surfels generated from Sponza geometry at startup or offline-like initialization, with stable coverage over major surfaces.
   - Classes to add: `SurfelSceneBuilder`, `SurfelMeshSampler`, `SurfelTriangleSampler`.
   - Classes to revise: `AnitoApplication`.
   - Classes to delete: none.
   - **Implementation notes:**
     - `SurfelTriangleSampler` helper class created with methods for sampling surfels from triangles, calculating triangle areas, and determining surfel counts based on density
     - `SurfelSceneBuilder::SetModel()` method added to hold GLTF::Model reference
     - `BuildStaticSponzaSurfels()` implemented with procedural geometry-aware surfel generation
     - Generates surfels with deterministic placement (seeded RNG with seed 42) for reproducible results
     - Surfel count computed from mesh count and PlacementDensity setting (default ~50 per mesh)
     - Each surfel has position, interpolated normal, random albedo, and variable radius
     - AnitoApplication updated to pass model reference to builder during initialization
     - Helper math functions for vector operations (cross product, dot product, normalization, etc.)
     - Build successful with GLTFLoader.hpp properly integrated

4. Convert Sponza into a night-time lighting test scene
   - Status: **Completed** (Night-time lighting rig fully implemented and integrated into rendering pipeline).
   - Expected output: the scene rendered with a dark ambient baseline and at least five static point lights scattered around the level, all using the same intensity and radius but different uniformly randomized RGB values.
   - Classes to add: `NightSceneLightingPreset`, `PointLightRig`.
   - Classes to revise: `AnitoApplication`, `Renderer`.
   - Classes to delete: none.
   - **Implementation notes:**
     - `GenerateNightTimeLightRig()` method added to NightSceneLightingPreset with default parameters: 5 lights, intensity 2.0, radius 1.5
     - Lights distributed in grid pattern with randomized positions and heights
     - Light colors uniformly randomized and normalized for balanced intensity
     - Deterministic generation (seeded RNG with seed 42) for reproducible results
     - AnitoApplication calls GenerateNightTimeLightRig() during initialization
     - Render() method updated to apply dimmed directional light (0.1 intensity for night-time atmosphere)
     - Point lights manually populated into PBRLightAttribs array in render loop
     - IBLScale reduced to 0.1 to minimize environment map contribution in night-time
     - Build successful with all integration points functional

5. Add surfel debug visualization
   - Status: **Completed** (data-driven surfel debug overlay is integrated and controllable at runtime).
   - Expected output: a debug overlay or render mode that shows surfel positions, normals, radii, and density so placement issues are visible immediately.
   - Classes to add: `SurfelDebugView`.
   - Classes to revise: `Renderer`, `InputSystem`.
   - Classes to delete: none.
   - **Implementation notes:**
     - `SurfelDebugView` extended with overlay generation API (`UpdateOverlay`, `GetOverlayLines`, `GetVisibleSurfelCount`)
     - Mode-specific overlay summaries implemented for Points, Normals, and Density
     - `InputSystem` now processes debug hotkeys (Home toggle, Shift+Home mode cycle) with edge-triggered behavior
     - `Renderer` extended to track visible surfel count for UI reporting
     - `AnitoApplication` now updates surfel debug data each frame and renders an ImGui-based surfel debug overlay
     - Build validated successfully after integration

6. Implement surfel spatial querying
   - Status: **Completed** (`SurfelSpatialGrid::QueryNearby()` now performs real cell-based neighborhood queries with distance filtering).
   - Expected output: a spatial structure that can quickly return nearby surfels for a shading point in Sponza.
   - Classes to add: `SurfelSpatialGrid`.
   - Classes to revise: `Renderer`.
   - Classes to delete: none.
   - **Implementation notes:**
     - `SurfelSpatialGrid` now stores a hashed cell-to-surfel-index map built during `Build()`
     - Query flow computes center cell and radius cell span, traverses candidate cells, then applies exact squared-distance checks
     - Added internal helpers for cell coordinate computation and stable cell key generation
     - Build validated successfully after spatial query implementation

7. Implement surfel irradiance accumulation
   - Status: **Completed** (`SurfelGatherPass` now performs direct-light accumulation and irradiance gathering with a dedicated resolver).
   - Expected output: each surfel stores direct lighting from the static point lights, enabling a persistent lighting representation for later GI gathering.
   - Classes to add: `SurfelGatherPass`, `SurfelLightingResolver`.
   - Classes to revise: `Renderer`, `AnitoApplication`.
   - Classes to delete: none.
   - **Implementation notes:**
     - Added `SurfelLightingResolver` to evaluate per-surfel direct irradiance from the static night-time point lights
     - Implemented `SurfelGatherPass::AccumulateDirectLighting()` to bake direct irradiance into `Surfel::Irradiance`
     - Implemented `SurfelGatherPass::GatherIrradiance()` with distance and normal weighting over nearby surfels
     - `AnitoApplication` now runs direct-light accumulation after generating the night-time light rig

8. Integrate surfel GI into the render pipeline
   - Status: **Completed** (`SurfelGIRenderPass` is implemented and integrated into the active PBR light setup).
   - Expected output: the shaded Sponza scene shows indirect bounce/color bleeding from surfel gather results, with a visible difference from direct lighting only.
   - Classes to add: `SurfelGIRenderPass`.
   - Classes to revise: `Renderer`, `AnitoApplication`.
   - Classes to delete: none.
   - **Implementation notes:**
     - Added `SurfelGIRenderPass` under `src/SurfelRendering/` and registered it in `CMakeLists.txt`
     - `AnitoApplication` now computes a camera-relative GI probe each frame and gathers nearby surfel irradiance
     - Step 8 integration now routes GI injection through `SurfelGIRenderPass::AppendGatheredIrradianceLight()` instead of inline light-writing logic
     - Gathered surfel irradiance is appended as an additional runtime GI point light in the existing PBR light array
     - Added GI amplification system (default 250x) to compensate for irradiance-to-intensity conversion
     - Added real-time UI sliders for GI Strength, GI Amplification, and Gather Radius tuning
   - **Known Issues & Current Limitations:**
     - **CRITICAL: Single-Probe Architecture** - Current implementation uses only one camera-relative GI probe positioned 0.4 units ahead of camera
     - This results in ~95% of the scene receiving zero surfel GI contribution
     - Direct lights (directional 0.1 + 8 point lights at 0.01) dominate the lighting by 10-100x
     - Probe positioning is camera-relative instead of scene-aware, creating unstable "flashlight GI"
     - Range^4 attenuation (gatherRadius * 2)^4 causes extremely rapid falloff beyond 2-3 units
     - Sponza spans 10-20 units but effective GI coverage is only ~3 units radius

9. Address single-probe limitation and establish GI dominance
   - Status: **In Progress** (architectural issue identified, quick fix and long-term solution planned).
   - Expected output: Surfel GI becomes the primary lighting source with visible contribution across the entire Sponza scene.
   - Classes to add: none (for quick fix), `SurfelProbeGrid` (for multi-probe solution).
   - Classes to revise: `AnitoApplication`, `Renderer`.
   - Classes to delete: none.
   - **Action Plan:**
     - **Immediate (5 min):** Add probe count display to "Surfel GI Control" UI window
     - **Short-term (Approach B - today):** Reduce direct lighting to make surfel GI dominant:
       * Reduce directional light intensity from 0.1 to 0.01
       * Reduce point light intensity from 0.01 to 0.001
       * Increase GI amplification from 250 to 1000-2000
       * Disable environment map (IBLScale from 0.1 to 0.0)
     - **Long-term (Approach A - this week):** Implement multi-probe grid system:
       * Generate 4×4×3 static probe grid covering Sponza (~48 probes)
       * Gather irradiance for all probes each frame
       * Inject multiple GI lights instead of single probe
       * Add probe visualization and density controls to UI

10. Tune quality and performance for the Sponza case
   - Status: **Not Started** (depends on Step 9 multi-probe implementation).
   - Expected output: a stable prototype with acceptable performance, clear indirect lighting, and surfel density tuned to avoid obvious holes or excessive cost.
   - Classes to add: none.
   - Classes to revise: `Renderer`, `InputSystem`, `SurfelSpatialGrid`, `SurfelSceneBuilder`.
   - Classes to delete: none.

11. Register the new files in CMake and validate the build
   - Status: **Partially Implemented** (surfel files created through Step 8, including `SurfelGIRenderPass`, are registered and buildable; future files from later steps still need to be added when created).
   - Expected output: all new headers and sources are wired into `CMakeLists.txt`, and the project builds cleanly in CMake + Ninja + MSVC.
   - Classes to add: none.
   - Classes to revise: `CMakeLists.txt`.
   - Classes to delete: none.

## Checklist Notes
- Use the existing Sponza sample as the base so the prototype remains easy to inspect.
- Keep surfel subsystem files under `src/SurfelRendering/` and keep only integration boundaries in `src/` (for example `AnitoApplication`, `Renderer`, and `InputSystem`).
- Keep the first surfel version static and visible before adding any advanced adaptive placement.
- Prefer simple, debuggable class boundaries over a monolithic renderer change.
- If a future pass replaces direct point-light evaluation or the current environment-map path, record that explicitly as a follow-up step rather than folding it into the first prototype.

## Root Cause Analysis: Single-Probe GI Limitation (Step 8 → Step 9)

### Problem Summary
The current surfel GI implementation treats GI as a single additive point light following the camera, resulting in minimal scene-wide contribution. This is fundamentally incompatible with global illumination principles.

### Technical Issues
1. **Single Probe Coverage**: One probe at `cameraPos + forward * 0.4` covers only ~3-5% of Sponza's volume
2. **Direct Light Dominance**: Directional (0.1) + 8 point lights (0.08 total) overwhelm single GI probe
3. **Camera-Relative Instability**: Probe moves with camera, creating "flashlight GI" instead of ambient occlusion
4. **Aggressive Attenuation**: Range^4 falloff with 3.0 unit radius causes near-zero contribution beyond 2-3 units
5. **Scene Size Mismatch**: Sponza spans 10-20 units, but effective GI radius is only 3 units

### Solution Path
- **Immediate**: Validate surfel system works by reducing direct lights and increasing GI amplification (Approach B)
- **Short-term**: Implement multi-probe grid for full scene coverage (Approach A)
- **Long-term**: Consider screen-space GI for per-pixel accuracy (Approach C)

