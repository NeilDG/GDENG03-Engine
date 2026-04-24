# PBR Test Scenes - Quick Reference Guide

## Scene 1: Sphere Grid - PBR Variation

**Description:** 5x5 grid demonstrating the full range of PBR parameters

**Layout:**
```
       Roughness (0.0 → 1.0) →
      
M  1.0  [●] [●] [●] [●] [●]  Fully Metallic
e  0.75 [●] [●] [●] [●] [●]
t  0.5  [●] [●] [●] [●] [●]
a  0.25 [●] [●] [●] [●] [●]
l  0.0  [●] [●] [●] [●] [●]  Non-Metallic
↓
```

**Key Observations:**
- **Top Row (Metallic = 1.0):** Strong colored reflections, mirror-like when smooth
- **Bottom Row (Metallic = 0.0):** Matte appearance, diffuse color dominates
- **Left Column (Roughness = 0.0):** Sharp, mirror-like reflections
- **Right Column (Roughness = 1.0):** Completely matte, no visible reflections

---

## Scene 2: Rotating Cubes - PBR Variation

**Description:** 5x5 grid of rotating cubes with varying PBR properties

**Features:**
- Each cube rotates with unique speed on X, Y, Z axes
- Same PBR parameter layout as Sphere Grid
- Demonstrates how lighting changes with surface orientation
- Shows specular highlights shifting across faces

**Purpose:**
- Validates PBR response to varying normals
- Tests BRDF evaluation at different angles
- Demonstrates Fresnel effect at grazing angles

---

## Scene 3: Mixed Materials Showcase

**Description:** 4x4 grid alternating between spheres and cubes

**Pattern:**
```
[●] [■] [●] [■]
[■] [●] [■] [●]
[●] [■] [●] [■]
[■] [●] [■] [●]
```

**Key Features:**
- Checkerboard pattern of geometry
- Same PBR parameter distribution as Scene 1
- Rotating cubes + static spheres
- Tests material consistency across different geometries

---

## Scene 4: Metallic Showcase (Conductors)

**Description:** Real-world metal materials with varying roughness

**Materials (Top Row → Bottom Row):**
1. **Gold** - `rgb(1.0, 0.85, 0.57)`
2. **Silver** - `rgb(0.97, 0.96, 0.91)`
3. **Copper** - `rgb(0.97, 0.74, 0.62)`
4. **Aluminum** - `rgb(0.91, 0.92, 0.92)`
5. **Iron** - `rgb(0.77, 0.78, 0.78)`
6. **Titanium** - `rgb(0.76, 0.73, 0.69)`
7. **Brass** - `rgb(0.98, 0.90, 0.59)`
8. **Platinum** - `rgb(0.83, 0.81, 0.78)`

**Roughness Range:** 0.0 to 0.6 (metals typically have lower roughness)

**Physical Basis:**
- Colors derived from measured complex IOR values
- Follows Filament's metal color specification
- Based on real-world spectral data

---

## Scene 5: Dielectric Showcase (Non-Metals)

**Description:** Common dielectric materials with varying roughness

**Materials (Top Row → Bottom Row):**
1. **Red Plastic** - `rgb(0.81, 0.0, 0.0)`
2. **Blue Plastic** - `rgb(0.0, 0.0, 0.81)`
3. **Green Plastic** - `rgb(0.0, 0.81, 0.0)`
4. **White Ceramic** - `rgb(0.9, 0.9, 0.9)`
5. **Wood** - `rgb(0.55, 0.35, 0.15)`
6. **Rubber** - `rgb(0.1, 0.1, 0.1)`
7. **Glass** - `rgb(0.9, 0.95, 1.0)`
8. **Stone** - `rgb(0.6, 0.6, 0.55)`

**Roughness Range:** 0.2 to 1.0 (dielectrics can be very rough)

**Characteristics:**
- Metallic = 0.0 for all materials
- Diffuse color dominates appearance
- Specular highlights are achromatic (white)
- Higher roughness typical for natural materials

---

## Material Parameter Guidelines (Filament Standard)

### Metallic Parameter:
- **0.0** = Pure dielectric (plastic, ceramic, wood)
- **1.0** = Pure conductor (metal)
- **0.0-1.0** = Use for blending (rust, oxidation, dirt)

### Roughness Parameter:
- **0.0** = Perfect mirror (polished metal, water)
- **0.2-0.4** = Slightly rough (polished wood, smooth plastic)
- **0.5-0.7** = Medium rough (concrete, unfinished wood)
- **0.8-1.0** = Very rough (rough fabric, clay, rubber)

### Base Color Guidelines:

**Dielectrics (Metallic = 0.0):**
- Represents diffuse albedo
- Typical range: sRGB 30-240
- Avoid very dark colors (<30) unless intentional
- Avoid pure white (>240) unless porcelain/snow

**Conductors (Metallic = 1.0):**
- Represents both specular color and reflectance
- Use measured values for realism
- Luminosity should be 67% to 100% (sRGB 170-255)
- Lower luminosity for oxidized/dirty metals

---

## Lighting Setup

**Directional Light:**
- Direction: `(-0.5, -1.0, -0.3)` normalized
- Intensity: `2.0` (multiplier)
- Type: Physically-based directional light
- Purpose: Simulates sun/primary light source

**Future Enhancements:**
- HDR environment map for IBL
- Additional point lights
- Spot lights for dramatic effects
- Area lights for soft shadows

---

## Camera Configuration

**Projection:**
- Field of View: 60°
- Aspect Ratio: Automatic (based on window size)
- Near Plane: 0.1 units
- Far Plane: 100 units

**Position (Dynamic per Scene):**
- Automatically calculated based on grid size
- Formula: `distance = maxDim * spacing * 0.8 + 5.0`
- Height: `maxDim * spacing * 0.3`
- Look-at: Scene center (0, 0, 0)

---

## Controls

| Key | Action |
|-----|--------|
| **SPACE** | Switch to next test scene |
| **ESC** | Exit application (standard) |

---

## Expected Visual Results

### What to Look For:

1. **Specular Highlights:**
   - Sharp on smooth metallic surfaces
   - Blurred on rough surfaces
   - Colored on metals, white on dielectrics

2. **Fresnel Effect:**
   - Surfaces become more reflective at grazing angles
   - Especially visible on smooth dielectric spheres

3. **Energy Conservation:**
   - Materials should never appear "glowing"
   - Bright areas = strong reflections, not emission

4. **Material Consistency:**
   - Same parameters → same appearance
   - Regardless of object shape (sphere vs cube)

5. **Rotating Cubes:**
   - Highlights move smoothly across faces
   - Intensity changes based on face orientation
   - No flickering or artifacts

---

## Troubleshooting

### Issue: Materials appear too dark
- Check light intensity setting
- Verify shader compilation
- Ensure base colors are in valid range

### Issue: No specular highlights visible
- Check roughness values (should have some smooth materials)
- Verify light direction
- Check camera position relative to light

### Issue: Metals look like plastic
- Verify metallic parameter is 1.0
- Check base color values (should be in metal range)
- Ensure shader properly handles metallic workflow

### Issue: Scene switching doesn't work
- Verify input system is initialized
- Check console for error messages
- Ensure GLFW key codes are correct

---

## Performance Benchmarks

**Target Performance (1920x1080):**
- Sphere Grid: 60 FPS (25 objects, ~12,800 tris)
- Cube Grid: 60+ FPS (25 objects, 300 tris)
- All scenes: < 5ms frame time

**Optimization Notes:**
- Mesh reuse significantly reduces memory
- Single shader reduces state changes
- Future: Instancing for massive performance gains

---

## Technical Notes

### Sphere Generation:
- 32 segments (longitude)
- 16 rings (latitude)
- Proper UV mapping
- Smooth normals for accurate lighting

### Cube Generation:
- 24 vertices (4 per face for proper normals)
- 36 indices (6 faces × 2 triangles × 3 vertices)
- Face-specific normals for flat shading

### Material Batching (Future):
- Group by shader
- Sort by material
- Batch by mesh type
- Instance identical objects

---

## Validation Checklist

Use this checklist when reviewing PBR implementation:

### Visual Quality:
- [ ] Smooth gradients between roughness values
- [ ] Clear difference between metallic and non-metallic
- [ ] No banding in color gradients
- [ ] Proper shadow falloff

### Physical Accuracy:
- [ ] Metals show colored reflections
- [ ] Dielectrics have white specular
- [ ] Fresnel effect visible at edges
- [ ] Energy conservation maintained

### Technical:
- [ ] No Z-fighting or rendering artifacts
- [ ] Smooth rotation on cubes
- [ ] Proper scene switching
- [ ] Stable frame rate

### Usability:
- [ ] Clear console feedback
- [ ] Intuitive controls
- [ ] Descriptive scene names
- [ ] Helpful scene descriptions

---

## References for Comparison

Compare Anito Engine output with these references:

1. **Filament Sample App**
   - Official Google Filament demos
   - Material playground

2. **Marmoset Toolbag**
   - Industry-standard PBR preview tool
   - Accurate material representation

3. **Substance Painter**
   - Real-time PBR preview
   - Wide range of material presets

4. **Unreal Engine**
   - High-quality PBR implementation
   - Reference for AAA quality

---

## Next Steps After Validation

Once all scenes render correctly:

1. **Capture reference screenshots**
2. **Document any visual discrepancies**
3. **Measure performance metrics**
4. **Begin Phase 4: HDR & IBL implementation**

---

*This guide is part of the Anito Engine - DOST-PCIEERD Project Anito*  
*Principal Investigator: Neil Patrick Del Gallego, Ph.D.*  
*DLSU GAME Lab - AAA Game Engine Development*
