# Anito Engine - Documentation Index

**Quick Navigation for Developers and AI Agents**

---

## 🚀 Getting Started

New to Anito Engine? Start here:

1. **[README.md](../README.md)** - Project overview and quick start
2. **[BUILD.md](BUILD.md)** - Build instructions and troubleshooting
3. **[QUICKSTART.md](../QUICKSTART.md)** - 5-minute setup guide
4. **[agents.md](../agents.md)** - AI agent instructions and coding standards

---

## 📖 Core Documentation

### System Design & Architecture

| Document | Description | Audience |
|----------|-------------|----------|
| **[ARCHITECTURE.md](ARCHITECTURE.md)** | System architecture, subsystems, data flow | All developers |
| **[ROADMAP.md](ROADMAP.md)** | Development roadmap and milestones | Project planning |
| **[IMPLEMENTATION_STATUS.md](../IMPLEMENTATION_STATUS.md)** | Current implementation status | All developers |

### Build & Configuration

| Document | Description | Audience |
|----------|-------------|----------|
| **[BUILD.md](BUILD.md)** | Complete build guide with troubleshooting | All developers |
| **[BUILD_INSTRUCTIONS.md](../BUILD_INSTRUCTIONS.md)** | Legacy detailed build instructions | Reference |
| **[RUNTIME_CONFIG.md](RUNTIME_CONFIG.md)** | Engine configuration (engine_config.ini) | All developers |
| **[ENGINE_CONFIG.md](ENGINE_CONFIG.md)** | Legacy config documentation | Reference |

### Rendering & Graphics

| Document | Description | Audience |
|----------|-------------|----------|
| **[SHADER_PIPELINE.md](SHADER_PIPELINE.md)** | Shader compilation and development | Graphics programmers |
| **[SHADER_COMPILATION_INSTRUCTIONS.md](../SHADER_COMPILATION_INSTRUCTIONS.md)** | Legacy shader instructions | Reference |

### Performance & Debugging

| Document | Description | Audience |
|----------|-------------|----------|
| **[PROFILING.md](PROFILING.md)** | Performance profiling and debugging tools | All developers |

---

## 🎓 Guides & Tutorials

Step-by-step guides for specific features:

| Guide | Topic | Level |
|-------|-------|-------|
| **[IBL_SETUP.md](guides/IBL_SETUP.md)** | Image-Based Lighting setup and troubleshooting | Intermediate |
| **[PBR_Test_Scenes_Guide.md](PBR_Test_Scenes_Guide.md)** | Testing PBR materials | Beginner |

---

## 🤖 AI Development

Documentation for AI-assisted development:

| Document | Purpose |
|----------|---------|
| **[agents.md](../agents.md)** | AI agent personas, responsibilities, protocols |
| **[CHANGELOG.md](../CHANGELOG.md)** | Version history and documentation changes |
| **[TODO.md](../TODO.md)** | Current development tasks |

**AI Feedback Loop:**
1. Run engine with profiling enabled
2. Analyze output in `anito-debug/`
3. AI parses JSON/CSV data
4. AI suggests optimizations
5. Apply fixes and re-profile

---

## 📦 Project Management

| Document | Description |
|----------|-------------|
| **[TODO.md](../TODO.md)** | Development task tracking |
| **[IMPLEMENTATION_STATUS.md](../IMPLEMENTATION_STATUS.md)** | Current status of all features |
| **[ROADMAP.md](ROADMAP.md)** | Long-term development plan |
| **[CHANGELOG.md](../CHANGELOG.md)** | Version history and changes |

---

## 📂 Directory Structure

```
docs/
├── index.md                          # This file
├── BUILD.md                          # Build guide
├── ARCHITECTURE.md                   # System architecture
├── SHADER_PIPELINE.md                # Shader development
├── PROFILING.md                      # Profiling system
├── ROADMAP.md                        # Development roadmap
├── RUNTIME_CONFIG.md                 # Runtime configuration
├── ENGINE_CONFIG.md                  # Legacy config docs
├── guides/                           # Step-by-step guides
│   └── IBL_SETUP.md                  # IBL guide
└── archive/                          # Historical documents
    ├── BUILD_SUCCESS.md
    ├── SESSION_SUMMARY.md
    ├── IBL_FIX_SUMMARY.md
    └── ... (30+ archived files)
```

---

## 🔍 Quick Reference

### Common Tasks

| Task | Documentation |
|------|---------------|
| **First time setup** | [BUILD.md](BUILD.md) → Setup section |
| **Build errors** | [BUILD.md](BUILD.md) → Troubleshooting section |
| **Shader compilation** | [SHADER_PIPELINE.md](SHADER_PIPELINE.md) |
| **PBR material setup** | [guides/IBL_SETUP.md](guides/IBL_SETUP.md) |
| **Performance profiling** | [PROFILING.md](PROFILING.md) |
| **Runtime config** | [RUNTIME_CONFIG.md](RUNTIME_CONFIG.md) |
| **Understanding architecture** | [ARCHITECTURE.md](ARCHITECTURE.md) |
| **Development roadmap** | [ROADMAP.md](ROADMAP.md) |

### By Role

**New Developer:**
1. README.md → Quick start
2. BUILD.md → Build instructions
3. ARCHITECTURE.md → Understand system design
4. TODO.md → Find tasks

**Graphics Programmer:**
1. SHADER_PIPELINE.md → Shader workflow
2. guides/IBL_SETUP.md → IBL implementation
3. ARCHITECTURE.md → Rendering pipeline section
4. PROFILING.md → GPU profiling

**AI Agent:**
1. agents.md → Agent instructions
2. ARCHITECTURE.md → System overview
3. PROFILING.md → Feedback loop setup
4. BUILD.md → Build validation

**Project Manager:**
1. IMPLEMENTATION_STATUS.md → Current status
2. ROADMAP.md → Future plans
3. CHANGELOG.md → Version history
4. TODO.md → Task tracking

---

## 📚 External References

### bgfx (Rendering)
- **Documentation:** https://bkaradzic.github.io/bgfx/
- **Examples:** `external/bgfx/examples/`
- **Shader Compiler:** https://bkaradzic.github.io/bgfx/tools.html#shader-compiler-shaderc

### PBR Theory
- **Physically Based Rendering Book:** https://pbr-book.org/
- **Filament Documentation:** https://google.github.io/filament/Filament.html
- **LearnOpenGL PBR:** https://learnopengl.com/PBR/Theory

### HDR Environments
- **Poly Haven:** https://polyhaven.com/hdris (Free CC0 HDRIs)
- **HDRI Haven:** https://hdrihaven.com/

---

## 🗂️ Archived Documentation

Historical documents (point-in-time snapshots) are preserved in `archive/`:

- **Status Reports:** Build success, session summaries, phase completions
- **Implementation Notes:** IBL fixes, shader solutions, compile error fixes
- **Redundant Guides:** Superseded by consolidated docs

**Access archived docs:** `docs/archive/`

**Note:** Archived files are kept for historical reference but may be outdated. Always use current documentation first.

---

## 📝 Documentation Conventions

### File Naming
- **UPPERCASE.md** - Core documents (README, CHANGELOG, TODO)
- **PascalCase.md** - Documentation (BUILD, ARCHITECTURE)
- **lowercase_with_underscores.md** - Guides (ibl_setup, getting_started)

### Sections
- Use `---` for major section breaks
- Use `##` for main sections, `###` for subsections
- Include table of contents for long documents

### Code Blocks
- Always specify language: ` ```cpp `, ` ```powershell `, ` ```ini `
- Include comments for complex code
- Show full working examples when possible

### Links
- Use relative links: `[BUILD.md](BUILD.md)`
- Link to specific sections: `[Build System](BUILD.md#build-system)`
- External links: Full URL

---

## 🆘 Need Help?

1. **Check documentation** - Search this index
2. **Review archived docs** - Historical context in `archive/`
3. **Check GitHub Issues** - Known issues and discussions
4. **Read agents.md** - AI agent guidelines
5. **Contact project lead** - Neil Patrick Del Gallego, Ph.D.

---

## 🔄 Document Maintenance

**Last Updated:** April 2026  
**Last Major Consolidation:** April 2026

**Maintenance Guidelines:**
- Update documentation when adding features
- Archive outdated documents (don't delete)
- Keep README.md and index.md synchronized
- Use CHANGELOG.md to document major changes
- Review documentation quarterly

**Contributing:**
- Follow existing format and style
- Update this index when adding new docs
- Test all code examples before committing
- Include screenshots/diagrams for complex topics

---

## Quick Links

- **Project Repository:** https://github.com/NeilDG/GDENG03-Engine
- **DLSU GAME Lab:** http://www.dlsu.edu.ph/
- **DOST-PCIEERD:** https://pcieerd.dost.gov.ph/
