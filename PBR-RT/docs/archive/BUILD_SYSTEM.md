# Build System - AAA Studio Style

## Overview

The Anito Engine build system has been redesigned following **AAA game studio best practices**, consolidating multiple scripts into a **single unified build system** similar to:

- **Unreal Engine**: `RunUAT.bat` with commands
- **CryEngine/Lumberyard**: Single build script with subcommands  
- **Unity**: Unified build tools

## What Changed

### Before (Multiple Scripts)
```
setup_build.bat
build.bat
clean.bat
check_dependencies.bat
tools/build_shaderc.bat
```

### After (Single Unified Script)
```
Build.bat [command] [options]
```

## Usage

### Basic Commands

```bash
# First time setup
Build.bat setup

# Build (default command)
Build.bat
Build.bat build

# Clean and rebuild
Build.bat rebuild

# Clean build artifacts
Build.bat clean

# Check dependencies
Build.bat check

# Build and run
Build.bat run

# Show help
Build.bat help
```

### Build Options

```bash
# Debug build
Build.bat build --config Debug

# Release build (default)
Build.bat build --config Release

# Verbose output
Build.bat build --verbose

# Combined
Build.bat build --config Debug --verbose
```

### Shader Compiler

```bash
# The main Build.bat provides a helper
Build.bat shaderc

# For full shaderc build, use the dedicated script
tools\build_shaderc.bat

# Then rebuild to enable shaders
Build.bat rebuild
```

## Why This Approach?

### Industry Standard

AAA game studios use unified build systems because they:

1. **Reduce Confusion** - One script to remember
2. **Easier Onboarding** - New developers learn one interface
3. **Consistent Interface** - Same pattern as professional tools
4. **Self-Documenting** - `Build.bat help` shows everything
5. **Scalable** - Easy to add new commands

### User-Friendly

```bash
# Instead of remembering multiple scripts:
setup_build.bat
build.bat
clean.bat

# Users only need to remember:
Build.bat [command]
```

### Examples from Industry

**Unreal Engine**:
```bash
RunUAT.bat BuildCookRun -project=...
RunUAT.bat BuildPlugin -plugin=...
```

**CryEngine**:
```bash
cry_cmake.exe --setup
cry_cmake.exe --build
```

**Unity**:
```bash
unity-build --target Android --configuration Release
```

## Implementation Details

### Command Structure

The script uses a subcommand pattern:

```batch
Build.bat [command] [options]
    |         |         |
    |         |         └─ Optional flags (--config, --verbose)
    |         └─ Action to perform (setup, build, clean, etc.)
    └─ Main script
```

### Smart Defaults

- Default command is `build` if none specified
- Default config is `Release`
- Auto-runs setup if build directory missing

### Error Handling

- Clear error messages
- Suggests next steps
- Returns proper exit codes

## Migration Guide

### For Existing Users

Old way still works through the new system:

```bash
# Old way (multiple scripts)
setup_build.bat   →   Build.bat setup
build.bat         →   Build.bat build
clean.bat         →   Build.bat clean
```

### For New Users

Simply use:

```bash
Build.bat setup
Build.bat
Build.bat run
```

## File Structure

```
AnitoEngine/
├── Build.bat                    # ✨ NEW: Unified build system
├── tools/
│   └── build_shaderc.bat       # Specialized tool (kept separate)
├── CMakeLists.txt              # Enhanced with better detection
└── [removed: setup_build.bat, build.bat, clean.bat, check_dependencies.bat]
```

## Benefits

### For Developers

- ✅ Easier to remember
- ✅ Faster workflow
- ✅ Professional interface
- ✅ Similar to industry tools they know

### For Project

- ✅ Less maintenance overhead
- ✅ Cleaner repository
- ✅ Professional appearance
- ✅ Scalable architecture

### For Documentation

- ✅ Simpler to document
- ✅ Fewer pages needed
- ✅ Easier examples
- ✅ Self-documenting via `help`

## Future Extensibility

Easy to add new commands:

```batch
# Future commands could include:
Build.bat test        # Run tests
Build.bat package     # Create distribution
Build.bat docs        # Generate documentation
Build.bat android     # Build for Android
```

## Comparison: Before vs After

### Before
```bash
# User needs to know 5+ scripts
check_dependencies.bat         # Check
setup_build.bat               # Setup
build.bat                     # Build
clean.bat                     # Clean
tools\build_shaderc.bat      # Shaders
# Then remember to run CMake commands for options
```

### After
```bash
# User needs to know 1 script
Build.bat check      # Check
Build.bat setup      # Setup
Build.bat build      # Build
Build.bat clean      # Clean
Build.bat shaderc    # Shaders (helper)
Build.bat help       # Show all commands
```

## Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `setup` | Initial project setup | `Build.bat setup` |
| `build` | Build the project | `Build.bat build` |
| `rebuild` | Clean + build | `Build.bat rebuild` |
| `clean` | Remove artifacts | `Build.bat clean` |
| `check` | Verify dependencies | `Build.bat check` |
| `run` | Build and run | `Build.bat run` |
| `shaderc` | Shader compiler info | `Build.bat shaderc` |
| `help` | Show usage | `Build.bat help` |

## Options Reference

| Option | Values | Description |
|--------|--------|-------------|
| `--config` | Debug, Release | Build configuration |
| `--verbose` | (flag) | Detailed output |

## Quick Reference Card

```
╔═══════════════════════════════════════════════════════╗
║          Anito Engine - Build Commands                ║
╠═══════════════════════════════════════════════════════╣
║  First time:  Build.bat setup                         ║
║  Daily use:   Build.bat                              ║
║  Run it:      Build.bat run                           ║
║                                                        ║
║  Debug:       Build.bat build --config Debug          ║
║  Clean:       Build.bat rebuild                       ║
║  Help:        Build.bat help                          ║
╚═══════════════════════════════════════════════════════╝
```

---

**This unified build system follows AAA game industry standards, making Anito Engine more professional and easier to use.**
