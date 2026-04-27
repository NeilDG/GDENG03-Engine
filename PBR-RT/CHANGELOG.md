# Anito Engine - Changelog

**Documentation Consolidation - April 2026**

---

## [Unreleased] - Documentation Restructuring

### Added
- **docs/BUILD.md** - Unified build documentation
- **docs/ARCHITECTURE.md** - System architecture overview
- **docs/SHADER_PIPELINE.md** - Comprehensive shader guide
- **docs/PROFILING.md** - Complete profiling system documentation
- **docs/ROADMAP.md** - Development roadmap and milestones
- **docs/guides/IBL_SETUP.md** - Image-Based Lighting guide
- **docs/archive/** - Archive directory for historical documents

### Changed
- **README.md** - Updated with new documentation structure
- **docs/RUNTIME_CONFIG.md** - Renamed from RUNTIME_CONFIG_IMPLEMENTATION.md

### Archived (moved to docs/archive/)
Historical documents that were point-in-time snapshots or have been superseded:

**Status Reports:**
- BUILD_SUCCESS.md
- BUILD_COMPLETE.md
- SESSION_SUMMARY.md
- PHASE_3A_COMPLETE.md
- TASK_COMPLETE_RUNTIME_CONFIG.md

**IBL Implementation Notes:**
- IBL_FINAL_FIX_BGFX_PATTERN.md
- IBL_FIX_COMPLETE_REPORT.md
- IBL_FIX_IMPLEMENTATION_REPORT.md
- IBL_FIX_SUMMARY.md
- IBL_TESTING_GUIDE.md
- docs/IBL_FINAL_IMPLEMENTATION.md
- docs/IBL_FIX_SUMMARY.md
- docs/IBL_IMPLEMENTATION_COMPLETE.md
- docs/IBL_INTEGRATION_SUMMARY.md

**Build Documentation (consolidated):**
- BUILDING.md
- BUILD_SYSTEM.md
- docs/BUILD_SYSTEM_SYNC.md

**Shader Documentation (consolidated):**
- SHADER_SETUP.md
- SHADER_SETUP_GUIDE.md
- FINAL_SHADER_SOLUTION.md
- docs/SHADER_COMPILATION.md

**Profiling Documentation (consolidated):**
- PROFILING_QUICK_START.md
- PROFILING_SYSTEM_DOCUMENTATION.md

**Runtime Config (consolidated):**
- docs/RUNTIME_CONFIG_QUICKREF.md
- docs/RUNTIME_CONFIG_VISUAL_GUIDE.md

**Miscellaneous:**
- FIX_COMPILE_ERRORS.md
- IMPROVEMENTS.md
- QUICK_REFERENCE.md
- SELF_REFLECTION_CHECKLIST.md
- Instruction Template.md

### Rationale

**Before:** 30+ markdown files with significant duplication and fragmentation
**After:** 10 core documents + organized guides + archive

**Benefits:**
✅ Single source of truth for each topic
✅ Easier maintenance (update one place, not five)
✅ Clearer navigation for developers and AI agents
✅ Follows industry standards (similar to Godot, Unreal, etc.)
✅ Historical documents preserved in archive for reference

---

## [Phase 2] - PBR & IBL Implementation - April 2026

### Added
- Complete PBR material system (metallic/roughness workflow)
- Image-Based Lighting (IBL) with HDR environments
- Deferred rendering pipeline
- Comprehensive profiling system:
  - CPU profiler (hierarchical timing)
  - GPU profiler (render pass timing)
  - Memory profiler (allocation tracking)
  - Console logger (dual output)
  - Frame capture system
  - Crash reporter (Windows minidumps)
- Runtime configuration system (engine_config.ini)
- 5 interactive PBR test scenes
- IBL toggle and scene switching

### Technical Achievements
- HDR environment loading and cubemap conversion
- Irradiance map generation (diffuse IBL)
- Prefilter map generation (specular IBL with mipmaps)
- BRDF lookup texture
- Filmic tone mapping
- Gamma correction

---

## [Phase 1] - Core Foundation - December 2024

### Added
- Core engine architecture
- Math library (Vector2D/3D/4D, Matrix4x4 with GLM)
- Rendering foundation:
  - bgfx integration
  - Vertex/index buffer management
  - Shader loading system
  - Texture loading (2D, cubemaps, HDR)
  - Material system
- GameObject & Component system
- Transform hierarchy
- Window management (GLFW)
- Input system
- Build system:
  - Unified Build.bat script
  - CMake configuration
  - Git submodule management
- Procedural mesh generation (cube, sphere, plane)

---

## Version History

| Version | Date | Milestone |
|---------|------|-----------|
| **Phase 2** | April 2026 | PBR + IBL Complete |
| **Phase 1** | December 2024 | Core Foundation Complete |
| **Project Start** | June 2025 | DOST-PCIEERD Funding Approved |

---

## Future Milestones

See [ROADMAP.md](ROADMAP.md) for detailed future development plans.

**Next Phase (Q2-Q3 2026):**
- Shadow mapping (cascaded shadow maps)
- Advanced BRDF (Cook-Torrance microfacet)
- Screen-space techniques (SSAO, SSR)

---

## Documentation Changes

### April 2026 - Major Consolidation

**Goal:** Establish single source of truth for all documentation

**Changes:**
1. Created unified documentation structure in `/docs`
2. Consolidated 30+ files into 10 core documents
3. Archived historical point-in-time documents
4. Updated README.md with new structure
5. Established clear documentation hierarchy

**Impact:**
- Reduced documentation redundancy by ~70%
- Improved discoverability for new developers
- Aligned with industry best practices
- Better AI agent integration

---

## Notes

- All archived files remain accessible in `docs/archive/` for historical reference
- Git history preserves all previous versions
- Documentation will be updated as features are implemented
- See `agents.md` for AI-assisted development guidelines
