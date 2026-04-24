# Filament-Quality PBR Roadmap

## Phase 3: Foundation (COMPLETED ✅)

### Implemented:
- ✅ PBR material parameterization (metallic/roughness workflow)
- ✅ Test scene framework with 5 different scenarios
- ✅ Interactive scene switching system
- ✅ Physically accurate metal colors
- ✅ Proper material parameter ranges
- ✅ Dynamic object updates (rotating cubes)
- ✅ Automatic camera positioning

### Status:
All Phase 3 objectives completed. System ready for advanced rendering features.

---

## Phase 4: Advanced BRDF & Lighting

### Objectives:
Implement Filament's Cook-Torrance microfacet BRDF with proper energy conservation.

### Components to Implement:

#### 1. Specular BRDF (Priority: HIGH)
**Reference:** Filament Section "Specular BRDF"

```glsl
// Cook-Torrance Specular BRDF
vec3 Fr = (D * V * F) / (4 * NoV * NoL);
```

**Sub-tasks:**
- [ ] **D term: GGX Normal Distribution Function**
  - Implement `D_GGX(NoH, roughness)`
  - Add fp16 optimization using Lagrange's identity
  - Handle edge cases (NoH close to 1.0)
  
- [ ] **V term: Smith GGX Visibility Function**
  - Implement height-correlated Smith G term
  - Add fast approximation for mobile
  - Pre-compute common terms
  
- [ ] **F term: Schlick Fresnel Approximation**
  - Implement `F_Schlick(VoH, f0, f90)`
  - Add scalar optimization
  - Handle grazing angles properly

**Estimated Effort:** 3-4 days  
**Testing:** Compare against Filament reference renders

#### 2. Diffuse BRDF (Priority: MEDIUM)
**Reference:** Filament Section "Diffuse BRDF"

**Options:**
1. **Lambert (Simple, Current)**
   ```glsl
   vec3 Fd = baseColor / PI;
   ```
   
2. **Disney Diffuse (Better, Recommended)**
   ```glsl
   vec3 Fd = (baseColor / PI) * Disney_Diffuse(NoV, NoL, LoH, roughness);
   ```

**Tasks:**
- [ ] Implement Disney diffuse with retro-reflection
- [ ] Make it optional (quality vs performance)
- [ ] Test energy conservation

**Estimated Effort:** 1-2 days

#### 3. Energy Conservation (Priority: HIGH)
**Reference:** Filament Section "Energy loss in specular reflectance"

**Tasks:**
- [ ] Implement multiscattering compensation
- [ ] Generate/load DFG lookup table
- [ ] Add energy compensation term: `f0 * (1/r - 1)`
- [ ] Validate with white furnace test

**Estimated Effort:** 2-3 days

#### 4. Shader Improvements
**Tasks:**
- [ ] Port existing shaders to full Cook-Torrance
- [ ] Add proper uniform definitions
- [ ] Implement half-precision optimizations
- [ ] Add shader validation tests

**Estimated Effort:** 2-3 days

**Total Phase 4 Time:** ~2 weeks

---

## Phase 5: Image-Based Lighting (IBL)

### Objectives:
Implement high-quality environment lighting using HDR cubemaps.

### Components:

#### 1. Environment Map Pipeline (Priority: HIGH)
**Reference:** Filament Section "Image based lights"

**Tasks:**
- [ ] Load HDR environment maps (equirectangular)
- [ ] Convert equirect to cubemap
- [ ] Implement mip-chain generation
- [ ] Add RGBM or RGBE encoding

**File Formats:**
- Input: `.hdr`, `.exr` (OpenEXR)
- Runtime: Cubemap with mipmaps

**Estimated Effort:** 3-4 days

#### 2. Diffuse Irradiance (Priority: HIGH)
**Reference:** Filament Section "Diffuse BRDF integration"

**Two Approaches:**

**A. Pre-convolved Irradiance Map:**
```glsl
vec3 irradiance = textureCube(irradianceMap, N);
vec3 diffuse = diffuseColor * irradiance * Fd_Lambert();
```

**B. Spherical Harmonics (Recommended for mobile):**
```glsl
vec3 irradiance = evaluateSH(N, shCoefficients);
```

**Tasks:**
- [ ] Implement cubemap convolution (offline tool)
- [ ] Or implement SH projection (9 coefficients)
- [ ] Optimize for runtime evaluation
- [ ] Test with various environment maps

**Estimated Effort:** 4-5 days

#### 3. Specular Pre-filtered Environment (Priority: HIGH)
**Reference:** Filament Section "Specular BRDF integration"

**Implementation:**
```glsl
// Split-sum approximation
vec3 prefilteredColor = textureCubeLod(envMap, R, roughness * maxLod);
vec2 envBRDF = textureLod(brdfLut, vec2(NoV, roughness), 0.0);
vec3 specular = prefilteredColor * (f0 * envBRDF.x + f90 * envBRDF.y);
```

**Tasks:**
- [ ] Pre-filter environment at different roughness levels
- [ ] Store in mipmap chain
- [ ] Generate DFG lookup table (128x128 or 256x256)
- [ ] Implement split-sum approximation in shader
- [ ] Handle edge cases (v=n assumption)

**DFG LUT Generation:**
- Offline tool using importance sampling
- Store as RG16F texture
- Pre-compute for various NoV and roughness values

**Estimated Effort:** 5-6 days

#### 4. IBL System Integration
**Tasks:**
- [ ] Create `AnitoEnvironmentMap` class
- [ ] Implement hot-reloading of environment maps
- [ ] Add intensity and rotation controls
- [ ] Integrate with material system
- [ ] Add debug visualization (show cubemap)

**Estimated Effort:** 2-3 days

**Total Phase 5 Time:** ~3 weeks

---

## Phase 6: Advanced Material Models

### 1. Clear Coat (Priority: MEDIUM)
**Reference:** Filament Section "Clear coat model"

**Use Cases:**
- Car paints
- Lacquered wood
- Coated metals

**Implementation:**
```glsl
// Second specular lobe for clear coat
vec3 Fcc = D_GGX(NoH_cc, roughness_cc) * 
           V_Kelemen(LoH_cc) * 
           F_Schlick(0.04, VoH_cc) * 
           clearCoat;

// Attenuate base layer
vec3 baseLayer = (Fd + Fr) * (1.0 - Fcc);
vec3 final = baseLayer + Fcc;
```

**Tasks:**
- [ ] Add clear coat parameters to material
- [ ] Implement Kelemen visibility function
- [ ] Handle energy attenuation of base layer
- [ ] Test with car paint materials

**Estimated Effort:** 3-4 days

### 2. Anisotropy (Priority: MEDIUM)
**Reference:** Filament Section "Anisotropic model"

**Use Cases:**
- Brushed metals
- Hair/fur
- Fabrics

**Implementation:**
```glsl
// Anisotropic GGX
float D_GGX_Anisotropic(NoH, ToH, BoH, at, ab);
float V_SmithGGXCorrelated_Anisotropic(...);
```

**Tasks:**
- [ ] Add anisotropy parameter
- [ ] Implement anisotropic NDF
- [ ] Add tangent/bitangent support
- [ ] Test with brushed metal materials

**Estimated Effort:** 4-5 days

### 3. Cloth/Fabric (Priority: LOW)
**Reference:** Filament Section "Cloth model"

**Use Cases:**
- Character clothing
- Furniture
- Soft materials

**Implementation:**
```glsl
// Velvet/Charlie NDF
float D_Charlie(roughness, NoH);
// Separate sheen color
vec3 Fr = D_Charlie * sheenColor;
```

**Tasks:**
- [ ] Implement cloth NDF (Charlie or Ashikhmin)
- [ ] Add sheen color parameter
- [ ] Add subsurface color for fabric
- [ ] Test with velvet, denim materials

**Estimated Effort:** 3-4 days

### 4. Subsurface Scattering (Priority: LOW)
**Reference:** Filament Section "Subsurface model"

**Use Cases:**
- Skin
- Wax
- Marble
- Translucent materials

**Implementation Options:**
1. **Wrapped Diffuse (Cheap)**
2. **Screen-Space Subsurface Scattering (Better)**
3. **Full BSSRDF (Expensive)**

**Estimated Effort:** 1-2 weeks (depending on approach)

**Total Phase 6 Time:** ~3-4 weeks

---

## Phase 7: Advanced Lighting

### 1. Punctual Lights (Priority: HIGH)
**Reference:** Filament Section "Punctual lights"

**Light Types:**
- Point lights
- Spot lights  
- Area lights (future)

**Physical Units:**
- Luminous power (lumens) for point/spot
- Illuminance (lux) for directional
- Luminous intensity (candela) for photometric

**Tasks:**
- [ ] Implement point light with inverse square falloff
- [ ] Add spot light with angular attenuation
- [ ] Implement windowed falloff for influence radius
- [ ] Add photometric light profiles (IES)
- [ ] Use physical light units throughout

**Estimated Effort:** 1 week

### 2. Shadow Mapping (Priority: HIGH)

**Types:**
- Directional (CSM for large scenes)
- Point (cubemap shadows)
- Spot (standard shadow map)

**Quality Features:**
- PCF filtering
- PCSS (soft shadows)
- Contact hardening

**Tasks:**
- [ ] Implement basic shadow mapping
- [ ] Add Cascaded Shadow Maps for directional
- [ ] Implement PCF filtering
- [ ] Add shadow bias controls
- [ ] Optimize for mobile (lower resolution, etc.)

**Estimated Effort:** 2 weeks

### 3. Clustered Forward Rendering (Priority: MEDIUM)
**Reference:** Filament uses tiled/clustered approaches

**Benefits:**
- Support hundreds of lights
- Efficient on mobile
- Better than deferred on mobile

**Tasks:**
- [ ] Implement frustum clustering (froxels)
- [ ] Light assignment to clusters
- [ ] Modify shaders for clustered access
- [ ] Benchmark against current forward

**Estimated Effort:** 2-3 weeks

**Total Phase 7 Time:** ~5-6 weeks

---

## Phase 8: Post-Processing & Camera

### 1. Physically-Based Camera (Priority: HIGH)
**Reference:** Filament Section "Physically based camera"

**Parameters:**
- Aperture (f-stops)
- Shutter speed (seconds)
- ISO sensitivity
- Exposure compensation

**Tasks:**
- [ ] Implement exposure calculation
- [ ] Add auto-exposure system
- [ ] Support exposure value (EV) system
- [ ] Add pre-exposure optimization

**Estimated Effort:** 1 week

### 2. Tone Mapping (Priority: HIGH)
**Reference:** Filament Section "Imaging pipeline"

**Operators:**
- ACES (Recommended)
- Filament (Custom)
- Uncharted 2
- Reinhard

**Tasks:**
- [ ] Implement ACES tone mapping
- [ ] Add color grading (LUT support)
- [ ] Implement white balance
- [ ] Add exposure controls

**Estimated Effort:** 1 week

### 3. Bloom (Priority: MEDIUM)

**Implementation:**
- Threshold bright areas
- Gaussian blur pyramid
- Composite back to scene

**Tasks:**
- [ ] Implement bloom extraction
- [ ] Add efficient blur (separable Gaussian)
- [ ] Add bloom intensity controls
- [ ] Optimize for mobile

**Estimated Effort:** 3-4 days

### 4. Anti-Aliasing (Priority: MEDIUM)

**Options:**
- MSAA (hardware, expensive on mobile)
- TAA (temporal, best quality/cost)
- FXAA (post-process, fast)

**Recommendation:** TAA for PC, FXAA for mobile

**Estimated Effort:** 1 week per technique

**Total Phase 8 Time:** ~4 weeks

---

## Phase 9: Optimization & Polish

### 1. Performance Optimization

**Tasks:**
- [ ] GPU profiling and optimization
- [ ] Shader hot-reloading for iteration
- [ ] Batch rendering optimization
- [ ] Instancing for repeated objects
- [ ] Frustum culling
- [ ] Occlusion culling
- [ ] Level-of-detail (LOD) system

**Estimated Effort:** 2-3 weeks

### 2. Mobile Optimization

**Tasks:**
- [ ] Vulkan mobile backend validation
- [ ] Reduce shader complexity (preprocessor variants)
- [ ] Texture compression (ASTC/ETC2)
- [ ] Memory optimization
- [ ] Battery/thermal considerations

**Estimated Effort:** 2-3 weeks

### 3. Quality-of-Life Features

**Tasks:**
- [ ] Material editor UI
- [ ] Environment map browser
- [ ] Light placement tools
- [ ] Real-time shader editing
- [ ] Performance overlay

**Estimated Effort:** 2-3 weeks

**Total Phase 9 Time:** ~6-8 weeks

---

## Complete Roadmap Timeline

| Phase | Focus | Duration | Priority |
|-------|-------|----------|----------|
| Phase 3 | Foundation & Testing | ✅ DONE | HIGH |
| Phase 4 | Advanced BRDF | 2 weeks | HIGH |
| Phase 5 | Image-Based Lighting | 3 weeks | HIGH |
| Phase 6 | Material Models | 3-4 weeks | MEDIUM |
| Phase 7 | Advanced Lighting | 5-6 weeks | HIGH |
| Phase 8 | Post-Processing | 4 weeks | HIGH |
| Phase 9 | Optimization | 6-8 weeks | HIGH |

**Total Estimated Time:** ~25-30 weeks (~6-7 months)

---

## Filament Features Not in Initial Scope

### Advanced (Future Consideration):

1. **Real-Time Ray Tracing**
   - Hybrid raster + ray tracing
   - RT shadows, reflections, GI
   - Requires RTX/DXR support

2. **Global Illumination**
   - Light probes (local and distant)
   - Irradiance volumes
   - Voxel-based GI

3. **Volumetrics**
   - Fog and atmospheric scattering
   - Volumetric lighting
   - Participating media

4. **Advanced Post-Effects**
   - Motion blur
   - Depth of field
   - Lens flares (physically-based)
   - Color fringing

---

## Success Metrics

### Visual Quality:
- [ ] Match Filament sample renders (side-by-side comparison)
- [ ] Pass white furnace test (energy conservation)
- [ ] Accurate representation of common materials
- [ ] Proper Fresnel at grazing angles

### Performance:
- [ ] 60 FPS @ 1920x1080 on mid-range GPU (GTX 1660)
- [ ] 30 FPS @ 1280x720 on mobile (Snapdragon 865)
- [ ] Scene with 100+ objects + 50+ lights
- [ ] < 8ms render time for single frame

### Technical:
- [ ] Shader complexity < 200 instructions (fragment)
- [ ] Memory usage < 512MB for test scenes
- [ ] Startup time < 2 seconds
- [ ] No visual artifacts (z-fighting, aliasing, etc.)

---

## Testing Strategy

### Per-Phase Validation:

1. **Reference Comparison**
   - Capture identical scenes in Filament
   - Side-by-side image comparison
   - Measure visual difference (MSE, SSIM)

2. **Performance Profiling**
   - GPU frame capture (RenderDoc, Nsight)
   - CPU profiling (Tracy, Optick)
   - Memory profiling

3. **Physical Validation**
   - White furnace test
   - Light measurement (if possible)
   - Energy conservation checks

4. **User Testing**
   - Industry partners review
   - Interns/students feedback
   - Academic review by Dr. Del Gallego

---

## Resources & References

### Essential Reading:
1. **Filament Documentation** (Primary Reference)
   - https://google.github.io/filament/Filament.md.html

2. **SIGGRAPH Course Notes**
   - "Physically Based Shading in Film and Game Production" (2012-2023)
   - Real-Time Rendering courses

3. **Academic Papers**
   - Burley (2012) - Disney BRDF
   - Heitz (2014) - Smith G term
   - Kulla & Conty (2017) - Multiscattering

4. **Industry Resources**
   - Unreal Engine documentation
   - Unity HDRP documentation
   - Marmoset Toolbag references

### Tools:
- RenderDoc (frame analysis)
- Nsight Graphics (NVIDIA profiling)
- cmgen (Filament's IBL tool - use as reference)
- Substance Designer/Painter (material authoring)

---

## Conclusion

This roadmap provides a structured approach to achieving Filament-quality PBR rendering in the Anito Engine. With Phase 3 complete, the foundation is solid for implementing advanced features.

**Recommended Approach:**
1. Complete Phase 4 (BRDF) first - biggest visual impact
2. Then Phase 5 (IBL) - enables realistic environment lighting
3. Optimize before adding more features (Phase 9 insights apply throughout)
4. Add advanced materials (Phase 6) as needed for specific scenes
5. Defer ray tracing and advanced GI until core features are rock-solid

**Key Principle:** Better to have fewer features working perfectly than many features working poorly. Filament's strength is its attention to quality and physical accuracy - we should match that standard.

---

*Anito Engine - DOST-PCIEERD Project Anito*  
*Principal Investigator: Neil Patrick Del Gallego, Ph.D.*  
*DLSU GAME Lab - AAA Game Engine Development*
