# Anito Engine - Profiling System Quick Start

**Phase 1 & 2 Implementation Complete** ✅

## Quick Integration Guide

### Step 1: Include the Profiler Manager

Add to your main engine initialization:

```cpp
#include "Debug/AnitoProfilerManager.h"

// In main() or engine initialization
AnitoProfilerManager::initialize(
    "anito-debug",                                    // Output directory
    true,                                             // Enable CPU profiling
    true,                                             // Enable GPU profiling
    AnitoMemoryProfiler::TrackingMode::LIGHT,        // Memory tracking (NONE, LIGHT, HEAVY)
    true                                              // Enable crash reporter
);
```

### Step 2: Add Frame Profiling to Game Loop

```cpp
void GameLoop() {
    AnitoProfilerManager::beginFrame();

    // Your game code here...
    Update(deltaTime);
    Render();

    AnitoProfilerManager::endFrame();
}
```

### Step 3: Add Scope Profiling (Optional but Recommended)

Define profiling flag in CMakeLists.txt or preprocessor:
```cmake
add_compile_definitions(ANITO_ENABLE_PROFILING)
```

Then use macros in your code:
```cpp
void MyRenderFunction() {
    ANITO_PROFILE_FUNCTION(); // Automatically uses function name

    // Or use custom scope names:
    {
        ANITO_PROFILE_SCOPE("Shadow Map Pass");
        // Shadow rendering code...
    }

    {
        ANITO_PROFILE_SCOPE("Lighting Pass");
        // Lighting code...
    }
}
```

### Step 4: Shutdown

```cpp
// Before program exit
AnitoProfilerManager::destroy();
```

## Output Files

All profiling data will be saved to:
```
anito-debug/
  ├── frames/          # Frame captures (TGA format)
  ├── logs/            # Console logs
  ├── profiling/       # Performance profiles (JSON/CSV)
  └── crashes/         # Crash dumps (if crash occurs)
```

## Logging Examples

```cpp
#include "Debug/AnitoConsoleLogger.h"

ANITO_LOG_INFO("Engine", "Engine initialized");
ANITO_LOG_WARNING("Renderer", "Low FPS detected");
ANITO_LOG_ERROR("Asset", "Failed to load texture");
ANITO_LOG_DEBUG("Physics", "Collision detected");
```

## Memory Tracking Examples

```cpp
#include "Debug/AnitoMemoryProfiler.h"

// Define this to enable memory tracking
#define ANITO_ENABLE_MEMORY_PROFILING

// Track allocations
void* myBuffer = malloc(1024);
ANITO_TRACK_ALLOC(myBuffer, 1024, "Renderer");

// Track deallocations
ANITO_TRACK_DEALLOC(myBuffer);
free(myBuffer);
```

## View Results

- **Logs:** Open `anito-debug/logs/anito_*.log` in any text editor
- **CPU Profile:** Open `anito-debug/profiling/cpu_profile_*.json` in any text editor or JSON viewer
- **Chrome Tracing:** Load `.json` file into `chrome://tracing` for visualization
- **Crash Dumps:** Open `.dmp` files in Visual Studio (File → Open → File)

## Performance Summary

At any time, print a performance summary:
```cpp
AnitoProfilerManager::printPerformanceSummary();
```

## Export All Profiles

```cpp
AnitoProfilerManager::exportAllProfiles("my_test_session");
```

## Full Documentation

See `PROFILING_SYSTEM_DOCUMENTATION.md` for complete documentation.

---

**Happy Profiling!** 🚀
