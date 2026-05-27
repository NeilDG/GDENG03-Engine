# Multi-Probe Surfel GI Implementation Plan (Approach A)

## Document Purpose
This plan defines the step-by-step implementation of a multi-probe surfel GI system to replace the current single camera-relative probe. This corresponds to **Approach A** from `surfel-implementation-plan.md` Step 9 and is the next major architectural evolution of the surfel prototype.

## Context & Prerequisites
- **Current State**: Single camera-relative GI probe positioned at `cameraPos + forward * 0.4`, providing ~3-5% scene coverage
- **Completed Foundation**: Static surfel baking, spatial queries, direct irradiance accumulation, and single-probe GI injection are all functional
- **Approach B Status**: Direct lighting has been reduced and GI amplification increased to validate surfel system correctness
- **Target Scene**: Sponza (~10-20 unit span, ~400-500 static surfels)
- **Goal**: Achieve 80-95% scene coverage with visible multi-bounce GI contribution

## Understanding
Implement a static 3D probe grid that covers the Sponza scene volume, with each probe gathering surfel irradiance independently and contributing as a separate GI light. This transforms the current "flashlight GI" (1 probe following camera) into true ambient lighting (48+ probes covering the entire scene).

## Assumptions
- Probe positions are computed once during initialization based on scene bounds
- Each probe gathers irradiance every frame using the existing `SurfelGatherPass::GatherIrradiance()` path
- All probes inject their gathered irradiance as separate point lights into Diligent's PBR light array
- The current `MAX_LIGHTS` limit in Diligent may need to be validated or increased
- Probe density (4×4×3 = 48 probes) is sufficient for Sponza's complexity without requiring per-frame adaptation
- Probe visualization is essential for debugging coverage and density issues

## Approach
The implementation follows a bottom-up strategy:
1. First, create the probe grid data structure and scene-aware probe placement logic
2. Then, integrate multi-probe gathering into the existing update loop (replacing single-probe logic)
3. Next, extend the GI render pass to inject multiple GI lights instead of one
4. Finally, add debug visualization and UI controls for probe density and coverage inspection

The key architectural change is in `AnitoApplication::UpdateSurfelPrototypeArchitecture()`, where the single probe position/gather/store flow becomes a loop over all probes in the grid. The spatial grid and gather pass remain unchanged—only their usage pattern shifts from 1-to-N probes.

## Key Files
- `src/SurfelRendering/SurfelProbeGrid.h/.cpp` - new probe grid subsystem (probe placement, bounds calculation, per-probe irradiance storage)
- `src/SurfelRendering/SurfelGIRenderPass.h/.cpp` - extend to inject N lights instead of 1
- `src/SurfelRendering/SurfelDebugView.h/.cpp` - add probe position/irradiance visualization mode
- `src/AnitoApplication.cpp` - replace single-probe logic with multi-probe loop, add probe grid initialization
- `src/AnitoApplication.h` - add `SurfelProbeGrid` member and probe visualization state
- `src/Renderer.h/.cpp` - store multi-probe irradiance data (replace single `std::array<float, 3>` with `std::vector`)
- `CMakeLists.txt` - register `SurfelProbeGrid.h/.cpp`

## Risks & Open Questions
- **Light Array Capacity**: Diligent's `MAX_LIGHTS` may be 32-64; injecting 48+ GI lights could exceed this limit. Mitigation: Start with 4×3×2 = 24 probes, validate, then scale up.
- **Performance**: 48 probes × ~20-50 nearby surfels per probe × per-frame gathering could impact frame time. Mitigation: Profile first, then consider caching or staggered updates.
- **Probe Placement**: Uniform grid may place probes outside navigable space or inside geometry. Mitigation: Use scene AABB from GLTFLoader, add optional per-probe validity checks.
- **Visual Quality**: Static probes may miss narrow corridors or require manual tuning. Mitigation: Start with conservative density, add runtime density controls.
- **Memory Overhead**: 48 probes × 3 floats irradiance × per-frame storage is minimal (~600 bytes), but probe spatial data and debug viz could add up. Mitigation: Keep probe struct minimal (position + irradiance only).

## Steps

### ✅ 1. Define the probe grid data contract [COMPLETED]
   **Goal**: Create `SurfelProbeGrid` class to store probe positions, compute scene bounds, and hold per-probe irradiance.

   **Status**: Completed - `SurfelProbeGrid.h/.cpp` created with `ProbeData` struct and full API

---

### ✅ 2. Implement scene bounds calculation from Sponza model [COMPLETED]
   **Goal**: Extract AABB from the loaded GLTF model to auto-compute probe grid bounds.

   **Status**: Completed - `ComputeSceneBounds()` implemented with hardcoded Sponza fallback

---

### ✅ 3. Integrate probe grid initialization into surfel prototype setup [COMPLETED]
   **Goal**: Initialize `SurfelProbeGrid` during `InitializeSurfelPrototypeArchitecture()` after surfel baking.

   **Status**: Completed - Probe grid initialized with runtime-configurable dimensions

---

### ✅ 4. Implement multi-probe irradiance gathering loop [COMPLETED]
   **Goal**: Replace single-probe gathering in `UpdateSurfelPrototypeArchitecture()` with a loop over all probes.

   **Status**: Completed - All probes gather irradiance each frame, single-probe logic retained for compatibility

---

### ✅ 5. Extend Renderer to store multi-probe irradiance data [COMPLETED]
   **Goal**: Replace single-irradiance storage with a vector of per-probe irradiance values.

   **Status**: Completed - `Renderer` stores vector-based multi-probe irradiance, legacy single-probe methods deprecated

---

### ✅ 6. Update SurfelGIRenderPass to inject multiple GI lights [COMPLETED]
   **Goal**: Extend `AppendGatheredIrradianceLight()` to loop over all probes and inject each as a separate light.

   **Status**: Completed - `AppendGatheredIrradianceLights()` injects multiple probe-derived lights with overflow checks

---

### ✅ 7. Add probe count and coverage validation to UI [COMPLETED]
   **Goal**: Display multi-probe status in the "Surfel GI Control" window.

   **Status**: Completed - UI shows probe grid dimensions, active/total probe count, and estimated coverage percentage

---

### ✅ 8. Implement probe debug visualization mode [COMPLETED]
   **Goal**: Add a "Probes" debug mode to visualize probe positions and gathered irradiance intensity.

   **Status**: Completed - `SurfelDebugMode::Probes` added with probe energy statistics and sample display

---

### ✅ 9. Add runtime probe density controls to UI [COMPLETED]
   **Goal**: Allow dynamic probe grid reconfiguration without recompiling.

   **Status**: Completed - Sliders for X/Y/Z dimensions, rebuild button, and overflow warnings implemented

---

### 10. Validate multi-probe GI light contribution [VALIDATION STEP]
   **Goal**: Verify that GI is now visible across the entire Sponza scene, not just near the camera.

   **Status**: Validation step — requires manual runtime testing by user

   **Validation Checklist**:
   - [ ] Console shows successful probe grid initialization with correct count
   - [ ] UI displays all probe controls and statistics (grid dimensions, active/total probes, coverage)
   - [ ] Active probe count > 20 out of 48 (or current grid total)
   - [ ] GI is visible in upper corridors, side rooms, and areas far from point lights
   - [ ] No "flashlight GI" effect when moving camera (lighting remains stable)
   - [ ] Probe debug mode displays energy statistics (min/avg/max non-zero)
   - [ ] GI Amplification slider visibly affects scene brightness (test 0 → 1000 → 3000)
   - [ ] Runtime grid rebuild works without crashes

   **Implementation Notes**:
   - This step requires running the application and performing visual/statistical inspection
   - Refer to the comprehensive validation guide provided for detailed checkpoints
   - Common issues include low active probe count (increase gather radius), over/under-amplification (tune sliders), or light count overflow (reduce probe density)

---
   **Goal**: Create `SurfelProbeGrid` class to store probe positions, compute scene bounds, and hold per-probe irradiance.

   **Expected Output**:
   - `src/SurfelRendering/SurfelProbeGrid.h` with:
	 * `struct ProbeData { std::array<float, 3> Position; std::array<float, 3> GatheredIrradiance; }`
	 * `void Initialize(const std::array<float, 3>& sceneMin, const std::array<float, 3>& sceneMax, int gridX, int gridY, int gridZ)`
	 * `const std::vector<ProbeData>& GetProbes() const`
	 * `std::size_t GetProbeCount() const`
   - `src/SurfelRendering/SurfelProbeGrid.cpp` with empty implementations

   **Classes to Add**: `SurfelProbeGrid`, `ProbeData` struct

   **Classes to Revise**: None

   **Implementation Notes**:
   - Probe positions are computed as a uniform 3D grid spanning `[sceneMin, sceneMax]`
   - Each probe stores its world position and a 3-component gathered irradiance value
   - Grid dimensions (e.g., 4×4×3) are passed as parameters to `Initialize()`
   - Probe spacing is `(sceneMax - sceneMin) / (gridDim - 1)` for each axis

---

### 2. Implement scene bounds calculation from Sponza model
   **Goal**: Extract AABB from the loaded GLTF model to auto-compute probe grid bounds.

   **Expected Output**:
   - `AnitoApplication::ComputeSceneBounds()` helper method that walks `m_Model` nodes/meshes
   - Returns `{minX, minY, minZ}` and `{maxX, maxY, maxZ}` for Sponza geometry
   - Fallback to hardcoded bounds `[-10, -5, -10]` to `[10, 5, 10]` if AABB walk is complex

   **Classes to Add**: None

   **Classes to Revise**: `AnitoApplication`

   **Implementation Notes**:
   - Diligent's `GLTF::Model` stores node transforms and mesh bounds; walk the scene graph to accumulate global AABB
   - If scene-graph traversal is too complex, start with hardcoded Sponza bounds and defer dynamic AABB to a later refinement step
   - Expand bounds by 10-20% to ensure edge coverage

---

### 3. Integrate probe grid initialization into surfel prototype setup
   **Goal**: Initialize `SurfelProbeGrid` during `InitializeSurfelPrototypeArchitecture()` after surfel baking.

   **Expected Output**:
   - `AnitoApplication.h` gains `SurfelProbeGrid m_SurfelProbeGrid;` member
   - `InitializeSurfelPrototypeArchitecture()` calls `m_SurfelProbeGrid.Initialize(sceneMin, sceneMax, 4, 4, 3)` after surfel baking
   - Log probe count and bounds: `LOG_INFO_MESSAGE("Initialized {count} GI probes covering [{minX},{minY},{minZ}] to [{maxX},{maxY},{maxZ}]")`

   **Classes to Add**: None

   **Classes to Revise**: `AnitoApplication`, `AnitoApplication.h`

   **Implementation Notes**:
   - Probe grid initialization happens once, after static surfel baking completes
   - Initial probe density is conservative: 4×4×3 = 48 probes for Sponza
   - Probe count is logged for validation and debugging

---

### 4. Implement multi-probe irradiance gathering loop
   **Goal**: Replace single-probe gathering in `UpdateSurfelPrototypeArchitecture()` with a loop over all probes.

   **Expected Output**:
   - Remove single-probe logic: `const float3 probePos = cameraPos + cameraForward * 0.4f;`
   - Add loop: `for (auto& probe : m_SurfelProbeGrid.GetProbes()) { ... }`
   - For each probe:
	 * Query nearby surfels: `QueryNearby(probe.Position, m_SurfelGIGatherRadius)`
	 * Gather irradiance: `GatherIrradiance(probe.Position, {0,1,0}, surfels, nearbyIndices, radius)`
	 * Store result: `probe.GatheredIrradiance = gatheredIrradiance`
   - Remove `m_SurfelGIProbePosition` and `m_SurfelGIProbeNormal` single-probe state

   **Classes to Add**: None

   **Classes to Revise**: `AnitoApplication`

   **Implementation Notes**:
   - Each probe uses a fixed upward normal `{0, 1, 0}` for hemispherical gathering (no directional bias)
   - Gathering happens every frame for all probes; profile this step if frame time exceeds budget
   - The existing `SurfelGatherPass::GatherIrradiance()` is reused without modification

---

### 5. Extend Renderer to store multi-probe irradiance data
   **Goal**: Replace single-irradiance storage with a vector of per-probe irradiance values.

   **Expected Output**:
   - `Renderer.h` changes:
	 * Remove: `std::array<float, 3> m_GatheredSurfelIrradiance`
	 * Add: `std::vector<std::array<float, 3>> m_GatheredSurfelIrradiances`
	 * Update: `void SetGatheredSurfelIrradiance(const std::vector<std::array<float, 3>>&)`
	 * Update: `const std::vector<std::array<float, 3>>& GetGatheredSurfelIrradiances() const`
   - `Renderer.cpp` updates getter/setter implementations
   - `AnitoApplication` calls `SetGatheredSurfelIrradiance()` with entire probe irradiance array after gathering loop

   **Classes to Add**: None

   **Classes to Revise**: `Renderer`, `AnitoApplication`

   **Implementation Notes**:
   - The irradiance vector size matches the probe count (e.g., 48 elements)
   - This separates per-probe irradiance from the single-probe `m_SurfelGIProbePosition` that is now deleted

---

### 6. Update SurfelGIRenderPass to inject multiple GI lights
   **Goal**: Extend `AppendGatheredIrradianceLight()` to loop over all probes and inject each as a separate light.

   **Expected Output**:
   - `SurfelGIRenderPass.h` signature changes:
	 * Old: `AppendGatheredIrradianceLight(..., const std::array<float, 3>& gatheredIrradiance, const std::array<float, 3>& probePosition, ...)`
	 * New: `AppendGatheredIrradianceLights(..., const std::vector<std::array<float, 3>>& probeIrradiances, const std::vector<std::array<float, 3>>& probePositions, ...)`
   - `SurfelGIRenderPass.cpp` loops over all probes:
	 * For each probe: check irradiance energy, compute light intensity, populate `PBRLightAttribs`, increment `totalLightCount`
	 * Early-exit loop if `totalLightCount >= MaxLights` to avoid buffer overflow
   - `AnitoApplication::Render()` passes probe data from `SurfelProbeGrid` instead of single probe position

   **Classes to Add**: None

   **Classes to Revise**: `SurfelGIRenderPass`, `AnitoApplication`

   **Implementation Notes**:
   - Each probe is injected as a point light with `Range` derived from `gatherRadius`
   - Light intensity uses the same `giStrength * giAmplification * irradiance` formula per probe
   - Log a warning if probe count exceeds available light slots

---

### 7. Add probe count and coverage validation to UI
   **Goal**: Display multi-probe status in the "Surfel GI Control" window.

   **Expected Output**:
   - "Surfel GI Control" window shows:
	 * `Active GI Probes: {count} (multi-probe grid)`
	 * `Probe Grid: {X}×{Y}×{Z} = {total}`
	 * `GI Coverage: ~{percentage}% (estimated)`
   - Tooltip: "Multi-probe grid provides scene-wide ambient lighting"
   - Remove or update the single-probe warning from Step 9

   **Classes to Add**: None

   **Classes to Revise**: `AnitoApplication`

   **Implementation Notes**:
   - Coverage percentage is estimated as `min(100, probeCount * 3)` for a rough heuristic (48 probes ≈ 100% coverage)
   - The UI now reflects the architectural shift from 1 camera-relative probe to N static probes

---

### 8. Implement probe debug visualization mode
   **Goal**: Add a "Probes" debug mode to visualize probe positions and gathered irradiance intensity.

   **Expected Output**:
   - `SurfelDebugMode` enum gains new entry: `Probes`
   - `SurfelDebugView::UpdateOverlay()` detects `Probes` mode and generates:
	 * One sphere or cross marker per probe at probe position
	 * Color intensity scales with gathered irradiance magnitude (dim = low GI, bright = high GI)
	 * Optional: draw small lines from probe toward dominant irradiance direction
   - Hotkey: `Shift+Home` cycles through modes now includes `Probes`

   **Classes to Add**: None

   **Classes to Revise**: `SurfelDebugView`, `InputSystem`, `AnitoApplication`

   **Implementation Notes**:
   - Probe visualization uses the same debug-line drawing path as surfel normals
   - Each probe marker is a small 3D cross (6 lines) or octahedron for visibility
   - Color is computed as `RGB = normalize(gatheredIrradiance) * length(gatheredIrradiance)` for hue + intensity

---

### 9. Add runtime probe density controls to UI
   **Goal**: Allow dynamic probe grid reconfiguration without recompiling.

   **Expected Output**:
   - "Surfel GI Control" window gains:
	 * `SliderInt("Probe Grid X", &gridX, 2, 8)` with tooltip "Horizontal probe density"
	 * `SliderInt("Probe Grid Y", &gridY, 2, 6)` with tooltip "Vertical probe density"
	 * `SliderInt("Probe Grid Z", &gridZ, 2, 8)` with tooltip "Depth probe density"
	 * Button: "Rebuild Probe Grid" → calls `m_SurfelProbeGrid.Initialize()` with new dimensions
   - Probe count updates immediately after rebuild, with a log message

   **Classes to Add**: None

   **Classes to Revise**: `AnitoApplication`, `AnitoApplication.h`

   **Implementation Notes**:
   - Probe grid rebuild is relatively cheap (no surfel rebake required)
   - Default values remain 4×4×3; users can experiment with denser grids (e.g., 6×5×4 = 120 probes)
   - Warn if probe count exceeds available light slots

---

### 10. Validate multi-probe GI light contribution
   **Goal**: Verify that GI is now visible across the entire Sponza scene, not just near the camera.

   **Expected Output**:
   - Scene shows ambient lighting in all rooms and corridors
   - Color bleeding from surfel irradiance is visible on surfaces far from the camera
   - No "flashlight GI" effect when moving the camera
   - Probe debug visualization shows non-zero irradiance in most probes

   **Classes to Add**: None

   **Classes to Revise**: None

   **Implementation Notes**:
   - Walk through Sponza and inspect corners, upper floors, and distant corridors
   - Compare before/after screenshots with single-probe vs. multi-probe lighting
   - Verify probe count in UI matches expected grid dimensions
   - Check console logs for warnings about light slot overflow

---

### 11. Register SurfelProbeGrid in CMakeLists.txt and build [COMPLETED]
   **Goal**: Add new source files to the build system and validate compilation.

   **Status**: Completed during Step 5 - `SurfelProbeGrid.h/.cpp` registered in `CMakeLists.txt`, build successful

---

### 12. Performance profiling and optimization [DEFERRED]
   **Goal**: Measure per-frame cost of multi-probe gathering and optimize if necessary.

   **Expected Output**:
   - Frame time breakdown showing probe gathering cost
   - If gathering exceeds 2-3ms: consider staggered updates (e.g., update 12 probes per frame in round-robin)
   - If spatial queries dominate: validate `SurfelSpatialGrid` cell size and query radius
   - Document final performance characteristics in plan notes

   **Classes to Add**: None

   **Classes to Revise**: `AnitoApplication` (optional: add frame timing logs or staggered update logic)

   **Implementation Notes**:
   - Use Diligent's profiler or manual `std::chrono` timing around the probe gathering loop
   - Baseline: 48 probes × 20 surfels × 60 FPS = ~57,600 surfel lookups/sec (should be fast)
   - If bottleneck found, consider caching probe irradiance for 2-3 frames or updating subsets per frame

---

## Success Criteria
- ✅ Multi-probe grid is initialized with configurable dimensions (default 4×4×3)
- ✅ All probes gather irradiance independently every frame using existing spatial queries
- ✅ Each probe injects a separate GI light into Diligent's PBR light array
- ✅ GI contribution is visible in all areas of Sponza, not just near the camera
- ✅ Probe debug visualization shows probe positions and irradiance intensity
- ✅ UI displays probe count, grid dimensions, and estimated coverage percentage
- ✅ Runtime probe density controls allow experimentation without recompiling
- ✅ Build compiles cleanly on CMake/Ninja/MSVC
- ✅ Frame time remains acceptable (< 5ms overhead for probe gathering on target hardware)

## Follow-Up Work (Deferred)
- **Adaptive Probe Placement**: Adjust probe density based on scene complexity or lighting gradients
- **Probe Validity Checks**: Disable probes inside solid geometry or outside navigable space
- **Screen-Space GI (Approach C)**: Replace multi-probe injection with per-pixel screen-space GI pass
- **Temporal Filtering**: Smooth probe irradiance over multiple frames to reduce flicker
- **Surfel Skinning**: Update surfel positions/normals for animated meshes
- **EA SEED-Style Placement**: Adaptive surfel spawning based on geometric detail

## References
- `surfel-implementation-plan.md` Steps 8-9 (single-probe baseline and root cause analysis)
- Existing surfel subsystem: `SurfelSceneBuilder`, `SurfelSpatialGrid`, `SurfelGatherPass`, `SurfelGIRenderPass`
- Diligent PBR Renderer: `GLTF_PBR_Renderer`, `PBRLightAttribs`, `MAX_LIGHTS` constant
- EA SEED GI papers (local `papers/` directory, if available)
