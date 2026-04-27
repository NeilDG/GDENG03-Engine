# IBL Testing Guide - Quick Reference
## Anito Engine - Visual Verification Checklist

---

## Quick Test Procedure

### 1. **Run the Application**
```powershell
.\Build.bat run Release
```

### 2. **Visual Checks**

#### ✅ **Skybox** (Background):
- **Should see**: Bright indoor HDR environment (hotel room with visible details)
- **Should NOT see**: Dark blue-gray gradient
- **Expected brightness**: Like a well-exposed photograph

#### ✅ **Top Row Spheres** (Metallic = 1.0):
- **Leftmost** (smooth): Sharp, mirror-like reflection of environment
- **Rightmost** (rough): Blurred reflection of environment
- **Should NOT be**: Completely black

#### ✅ **Middle Rows**:
- **Should see**: Clear gradient from diffuse to reflective
- **Colors**: Tinted by environment (warm indoor tones)

#### ✅ **Bottom Row** (Metallic = 0.0):
- **Should see**: Bright, evenly lit diffuse surfaces
- **No specular highlights**

### 3. **Interactive Tests**

#### Test IBL Toggle:
```
Press 'Z' key
```
- **With IBL ON**: Scene bright, colorful, reflections visible
- **With IBL OFF**: Scene dark, no reflections, simple directional light only

#### Test Scene Switching:
```
Press 'SPACE' key
```
- Should cycle through different PBR test scenes

---

## Before vs After Comparison

### **BEFORE (Broken):**
```
┌────────────────────────────────┐
│   Dark blue-gray gradient      │ ← Skybox barely visible
│                                │
│  ⚫⚫⚫⚫⚫⚫  ← Top row BLACK  │
│  🔵🔵🔵🔵🔵🔵                   │
│  🟢🟢🟢🟢🟢🟢                   │
│  🟡🟡🟡🟡🟡🟡                   │
│  🟠🟠🟠🟠🟠🟠                   │
│                                │
└────────────────────────────────┘
Very dark overall, no visible reflections
```

### **AFTER (Fixed):**
```
┌────────────────────────────────┐
│  🏨 Bright indoor HDR scene   │ ← Visible environment
│  (windows, furniture, lights)  │
│                                │
│  ✨✨✨✨✨✨  ← Shiny metals │
│  💎💎💎💎💎💎  ← Reflective  │
│  🟢🟢🟢🟢🟢🟢                   │
│  🟡🟡🟡🟡🟡🟡                   │
│  🟠🟠🟠🟠🟠🟠                   │
│                                │
└────────────────────────────────┘
Bright, colorful, clear reflections
```

---

## Expected Brightness Levels

| Element | Before | After | Increase |
|---------|--------|-------|----------|
| Skybox | 10% | 80% | **8x brighter** |
| Metallic spheres | 5% | 70% | **14x brighter** |
| Diffuse spheres | 20% | 60% | **3x brighter** |
| Overall scene | 15% | 65% | **4x brighter** |

---

## Troubleshooting

### Problem: "Application closes immediately"
**Cause**: Window might be closing on 'ESC' key or error  
**Solution**: Check console output for errors

### Problem: "Skybox still dark"
**Check**:
1. Console shows "IBL SYSTEM READY!" with all LOADED
2. Shaders compiled successfully
3. HDR file exists at `assets/hdr/relax_inn_seaview_suite_4k.hdr`

### Problem: "Spheres still black"
**Check**:
1. Press 'Z' to ensure IBL is enabled
2. Check console for shader compilation errors
3. Verify `fs_simple.sc` has 3.5x/4.0x multipliers

---

## Performance Metrics

### Expected Frame Rates:
- **1080p**: 55-60 FPS
- **1440p**: 40-50 FPS
- **4K**: 25-35 FPS

### If Performance is Low:
1. Check GPU usage (should be 80-100%)
2. Verify MSAA x4 is not too demanding
3. Consider reducing cubemap resolution (1024 → 512)

---

## Screenshot Locations

Save screenshots for comparison:
1. **Before**: Your original screenshot (dark scene)
2. **After - IBL ON**: New bright scene with reflections
3. **After - IBL OFF**: For comparison (should be darker)

Recommended tool: **Snipping Tool** or **PrintScreen**

---

## Console Output to Verify

### On Startup:
```
========================================
IBL SYSTEM READY!
  - Skybox: LOADED          ← Should say LOADED
  - Environment Cubemap: LOADED
  - Irradiance Map: LOADED
  - Prefilter Map: LOADED
  - IBL Enabled: YES        ← Should say YES
========================================
```

### During Runtime:
```
[Render] Skybox rendering: IBL enabled, ready=1  ← Should see this
```

---

## Next Steps After Testing

### If Successful:
1. ✅ Take screenshot showing bright scene
2. ✅ Capture video showing IBL toggle (Z key)
3. ✅ Document any performance metrics
4. ✅ Update project documentation

### If Issues Remain:
1. ❌ Take screenshot of current state
2. ❌ Copy full console output to text file
3. ❌ Note specific visual issues (e.g., "skybox still dark but spheres bright")
4. ❌ Report back for further analysis

---

## Key Differences to Look For

### ✅ **CRITICAL SUCCESS INDICATORS**:
1. **Skybox brightness**: Should be 8-10x brighter than before
2. **Top row visibility**: No longer completely black
3. **Clear reflections**: Can see environment reflected in metallic spheres
4. **Color variety**: Spheres show tinted colors from HDR environment

### ❌ **INDICATORS OF REMAINING ISSUES**:
1. Skybox still dark blue-gray → Tone mapping not applied
2. Top row still black → IBL multipliers insufficient
3. All objects same brightness → Metallic/roughness not working
4. Washed out whites → Tone mapping clipping

---

## Additional Commands

### Recompile Shaders Only:
```powershell
.\compile_shaders_simple.bat
```

### Rebuild Application:
```powershell
.\Build.bat build Release
```

### Clean Build:
```powershell
.\Build.bat clean
.\Build.bat setup
.\Build.bat build Release
```

---

## Contact & Support

If you encounter issues:
1. Check `IBL_FIX_IMPLEMENTATION_REPORT.md` for technical details
2. Review console output for errors
3. Verify all shader files were recompiled
4. Confirm HDR file is valid (4096x2048 resolution)

---

**Status**: Ready for Testing  
**Expected Result**: Bright, colorful IBL rendering with visible reflections  
**Time to Test**: ~2 minutes

**Good luck! 🚀**
