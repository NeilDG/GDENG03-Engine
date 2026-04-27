# Anito Engine - Profiling System

**Comprehensive Performance Analysis & Debugging**  
**DLSU GAME Lab - Project Anito**

---

## Table of Contents
1. [Overview](#overview)
2. [Quick Start](#quick-start)
3. [Profiler Components](#profiler-components)
4. [Configuration](#configuration)
5. [Output Analysis](#output-analysis)
6. [AI Feedback Loops](#ai-feedback-loops)

---

## Overview

The Anito Engine profiling system provides comprehensive performance monitoring for AAA game development:

- ✅ **CPU Profiling** - Hierarchical timing, frame analysis
- ✅ **GPU Profiling** - Render pass timing
- ✅ **Memory Profiling** - Allocation tracking (light/heavy modes)
- ✅ **Console Logging** - Dual console/file output with color coding
- ✅ **Frame Capture** - Automated PNG screenshots
- ✅ **Crash Reporting** - Windows minidump generation
- ✅ **JSON Export** - AI-ready data format

---

## Quick Start

### Step 1: Initialize Profiler Manager

Add to your engine initialization:

```cpp
#include "Debug/AnitoProfilerManager.h"

int main() {
    // Initialize all profilers
    AnitoProfilerManager::initialize(
        "anito-debug",                                    // Output directory
        true,                                             // Enable CPU profiling
        true,                                             // Enable GPU profiling
        AnitoMemoryProfiler::TrackingMode::LIGHT,        // Memory tracking mode
        true                                              // Enable crash reporter
    );

    // Your engine code...

    // Cleanup before exit
    AnitoProfilerManager::destroy();

    return 0;
}
```

### Step 2: Add Frame Profiling

Wrap your game loop:

```cpp
void GameLoop() {
    while (!shouldQuit) {
        // Begin frame profiling
        AnitoProfilerManager::beginFrame();

        // Your game logic
        processInput();
        updatePhysics(deltaTime);
        renderScene();

        // End frame profiling
        AnitoProfilerManager::endFrame();
    }
}
```

### Step 3: Enable Profiling Macros

**In CMakeLists.txt:**
```cmake
# Enable profiling in Debug and RelWithDebInfo builds
if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    add_compile_definitions(ANITO_ENABLE_PROFILING)
endif()
```

**Or in your code:**
```cpp
#define ANITO_ENABLE_PROFILING
#include "Debug/AnitoPerformanceProfiler.h"
```

### Step 4: Profile Your Code

```cpp
void renderScene() {
    ANITO_PROFILE_FUNCTION(); // Automatically uses function name

    {
        ANITO_PROFILE_SCOPE("Shadow Map Pass");
        renderShadowMaps();
    }

    {
        ANITO_PROFILE_SCOPE("Geometry Pass");
        renderGeometry();
    }

    {
        ANITO_PROFILE_SCOPE("Lighting Pass");
        renderLighting();
    }

    {
        ANITO_PROFILE_SCOPE("Post-Processing");
        applyPostProcessing();
    }
}
```

---

## Profiler Components

### 1. Console Logger

**Purpose:** Dual console/file logging with timestamps and color coding

#### Features
- Thread-safe logging
- Color-coded console output (ANSI codes)
- Automatic log file creation
- Session separation with timestamps

#### Usage

```cpp
#include "Debug/AnitoConsoleLogger.h"

// Initialize (called by ProfilerManager)
AnitoConsoleLogger::initialize("anito-debug/logs");

// Log messages
ANITO_LOG_INFO("Engine", "Engine initialized successfully");
ANITO_LOG_WARNING("Renderer", "Shader compilation took 15ms");
ANITO_LOG_ERROR("Asset", "Failed to load texture: " << filename);
ANITO_LOG_DEBUG("Physics", "Collision detected at " << position);

// Destroy (called by ProfilerManager)
AnitoConsoleLogger::destroy();
```

#### Log Levels

| Level | Color | Use Case |
|-------|-------|----------|
| **DEBUG** | Gray | Verbose diagnostic info |
| **INFO** | White | General information |
| **WARNING** | Yellow | Non-critical issues |
| **ERROR** | Red | Critical errors |

#### Output

**Console:**
```
[2026-04-15 14:23:45] [INFO] [Engine] Engine initialized successfully
[2026-04-15 14:23:45] [WARNING] [Renderer] Shader compilation took 15ms
[2026-04-15 14:23:45] [ERROR] [Asset] Failed to load texture: missing.png
```

**File:** `anito-debug/logs/anito_2026-04-15_14-23-45.log`

---

### 2. CPU Performance Profiler

**Purpose:** Hierarchical CPU timing analysis

#### Features
- Scoped profiling with RAII
- Hierarchical timing tree
- Min/Max/Average statistics
- Frame timing graph
- Export to JSON and Chrome Tracing format

#### Usage

```cpp
#include "Debug/AnitoPerformanceProfiler.h"

// Initialize (called by ProfilerManager)
AnitoPerformanceProfiler::initialize("anito-debug/profiling");

// Manual profiling
void update(float dt) {
    auto* profiler = AnitoPerformanceProfiler::getInstance();

    profiler->beginFrame();

    profiler->beginScope("AI Update");
    updateAI(dt);
    profiler->endScope();

    profiler->beginScope("Rendering");
    render();
    profiler->endScope();

    profiler->endFrame();
}

// Or use macros (recommended)
void update(float dt) {
    ANITO_PROFILE_FUNCTION();

    {
        ANITO_PROFILE_SCOPE("AI Update");
        updateAI(dt);
    }

    {
        ANITO_PROFILE_SCOPE("Rendering");
        render();
    }
}

// Export data
profiler->exportToJSON("profile.json", sessionDuration);
profiler->exportToChromeTracing("trace.json");

// Destroy (called by ProfilerManager)
AnitoPerformanceProfiler::destroy();
```

#### Output Formats

**JSON (AI-ready):**
```json
{
  "session_duration": 60.0,
  "total_frames": 3600,
  "scopes": [
    {
      "name": "Rendering",
      "call_count": 3600,
      "total_time_ms": 45000.0,
      "avg_time_ms": 12.5,
      "min_time_ms": 10.2,
      "max_time_ms": 18.7
    }
  ]
}
```

**Chrome Tracing (chrome://tracing):**
- Visualize hierarchical timing
- Zoom into individual frames
- Compare frame timing

---

### 3. GPU Profiler

**Purpose:** GPU render pass timing

#### Features
- Per-pass GPU timing
- Automatic profiling of bgfx views
- Export to CSV
- Minimal overhead

#### Usage

```cpp
#include "Debug/AnitoGPUProfiler.h"

// Initialize (called by ProfilerManager)
AnitoGPUProfiler::initialize("anito-debug/profiling");

// Automatic profiling (integrated with AnitoRenderer)
void render() {
    auto* gpuProfiler = AnitoGPUProfiler::getInstance();

    gpuProfiler->beginFrame();

    // View 0: Geometry
    bgfx::setViewName(0, "Geometry Pass");
    renderGeometry();

    // View 1: Lighting
    bgfx::setViewName(1, "Lighting Pass");
    renderLighting();

    gpuProfiler->endFrame();
}

// Export data
gpuProfiler->exportToCSV("gpu_profile.csv", sessionDuration);

// Destroy (called by ProfilerManager)
AnitoGPUProfiler::destroy();
```

#### Output (CSV)

```csv
View,Avg GPU Time (ms),Min GPU Time (ms),Max GPU Time (ms)
Geometry Pass,5.2,4.8,6.1
Lighting Pass,3.8,3.2,4.5
Post-Processing,2.1,1.8,2.5
```

---

### 4. Memory Profiler

**Purpose:** Memory allocation tracking

#### Features
- Light mode: Total allocations only
- Heavy mode: Per-allocation tracking with call stacks
- Peak memory usage
- Leak detection
- Export to JSON

#### Usage

```cpp
#include "Debug/AnitoMemoryProfiler.h"

// Initialize with tracking mode
AnitoMemoryProfiler::initialize(
    "anito-debug/profiling",
    AnitoMemoryProfiler::TrackingMode::LIGHT  // or HEAVY
);

// Memory is tracked automatically via overloaded new/delete

// Get stats
auto* memProfiler = AnitoMemoryProfiler::getInstance();
size_t currentUsage = memProfiler->getCurrentMemoryUsage();
size_t peakUsage = memProfiler->getPeakMemoryUsage();

// Export
memProfiler->exportToJSON("memory_profile.json");

// Destroy (reports leaks)
AnitoMemoryProfiler::destroy();
```

#### Tracking Modes

| Mode | Overhead | Features |
|------|----------|----------|
| **NONE** | 0% | No tracking |
| **LIGHT** | <1% | Total allocations only |
| **HEAVY** | 5-10% | Per-allocation tracking, call stacks |

**Recommendation:** Use LIGHT for Release, HEAVY for Debug

#### Output (JSON)

```json
{
  "current_memory_mb": 128.5,
  "peak_memory_mb": 256.3,
  "total_allocations": 12548,
  "total_deallocations": 12540,
  "active_allocations": 8,
  "leaks": [
    {
      "address": "0x12345678",
      "size_bytes": 1024,
      "callstack": "AnitoTexture::load() at Texture.cpp:45"
    }
  ]
}
```

---

### 5. Frame Capture System

**Purpose:** Automated screenshot capture

#### Features
- PNG format with proper orientation
- Automatic timestamped filenames
- Configurable capture interval
- Minimal performance impact

#### Usage

```cpp
#include "Debug/AnitoFrameCapture.h"

// Initialize
AnitoFrameCapture::initialize("anito-debug/frames");

// Configure capture interval
AnitoFrameCapture::getInstance()->setCaptureInterval(1.0f); // 1 second

// In game loop
void render() {
    // Render scene...

    // Capture frame if interval elapsed
    AnitoFrameCapture::getInstance()->update(deltaTime);
    AnitoFrameCapture::getInstance()->captureFrame();
}

// Destroy
AnitoFrameCapture::destroy();
```

#### Output

**Files:** `anito-debug/frames/frame_YYYY-MM-DD_HH-MM-SS.png`

---

### 6. Crash Reporter

**Purpose:** Windows minidump generation on crashes

#### Features
- Automatic crash detection
- Minidump generation (for Visual Studio debugging)
- Exception info logging
- Call stack capture

#### Usage

```cpp
#include "Debug/AnitoCrashReporter.h"

// Initialize (Windows only)
#ifdef _WIN32
    AnitoCrashReporter::initialize("anito-debug/crashes");
#endif

// Crash reporter runs automatically on unhandled exceptions

// Destroy
#ifdef _WIN32
    AnitoCrashReporter::destroy();
#endif
```

#### Output

**Files:** 
- `anito-debug/crashes/crash_YYYY-MM-DD_HH-MM-SS.dmp` (minidump)
- `anito-debug/crashes/crash_YYYY-MM-DD_HH-MM-SS.log` (exception info)

**Analyzing crashes:**
1. Open `.dmp` file in Visual Studio
2. Set symbol paths (Tools > Options > Debugging > Symbols)
3. View call stack at crash point

---

## Configuration

### Runtime Configuration (engine_config.ini)

```ini
[Profiling]
EnableCPUProfiling=true
EnableGPUProfiling=true
MemoryTrackingMode=1       ; 0=None, 1=Light, 2=Heavy
EnableCrashReporter=true
FrameCaptureInterval=1.0   ; Seconds between frame captures
OutputDirectory=anito-debug

[Logging]
LogLevel=1                 ; 0=Debug, 1=Info, 2=Warning, 3=Error
EnableConsoleOutput=true
EnableFileOutput=true
```

### Compile-Time Configuration

**CMakeLists.txt:**
```cmake
# Enable profiling in Debug builds
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(ANITO_ENABLE_PROFILING)
    add_compile_definitions(ANITO_ENABLE_MEMORY_PROFILING)
endif()

# Disable profiling in Release builds
if(CMAKE_BUILD_TYPE MATCHES "Release")
    # Profiling macros become no-ops
endif()
```

---

## Output Analysis

### Directory Structure

```
anito-debug/
├── frames/                          # Frame captures
│   ├── frame_2026-04-15_14-23-45.png
│   ├── frame_2026-04-15_14-23-46.png
│   └── ...
├── logs/                            # Console logs
│   ├── anito_2026-04-15_14-23-45.log
│   └── ...
├── profiling/                       # Performance profiles
│   ├── cpu_profile_2026-04-15_14-23-45.json
│   ├── gpu_profile_2026-04-15_14-23-45.csv
│   ├── memory_profile_2026-04-15_14-23-45.json
│   └── trace_2026-04-15_14-23-45.json (Chrome Tracing)
└── crashes/                         # Crash dumps (if any)
    ├── crash_2026-04-15_14-25-00.dmp
    └── crash_2026-04-15_14-25-00.log
```

### Analyzing CPU Performance

**Using Chrome Tracing (Recommended):**
1. Open Google Chrome
2. Navigate to `chrome://tracing`
3. Click "Load" and select `trace_*.json`
4. Zoom and pan to analyze frame timing

**Using JSON Data:**
- Parse JSON with Python/Node.js
- Identify bottlenecks (highest avg_time_ms)
- Compare frame timing across sessions

### Analyzing GPU Performance

**Using CSV Data:**
- Open in Excel/Google Sheets
- Sort by "Avg GPU Time" descending
- Identify expensive render passes

---

## AI Feedback Loops

### Purpose

Enable AI agents to:
1. **Analyze performance** from JSON/CSV data
2. **Detect bottlenecks** automatically
3. **Suggest optimizations** based on profiling data
4. **Verify fixes** by comparing before/after profiles

### Workflow

```
┌─────────────────┐
│   Run Engine    │  (with profiling enabled)
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Generate Data  │  (JSON, CSV, PNG)
│  anito-debug/   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  AI Analysis    │  (Python script or AI agent)
│  - Parse JSON   │
│  - Find issues  │
│  - Suggest fix  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Apply Fix      │  (AI or human developer)
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Re-profile     │  (verify improvement)
└─────────────────┘
```

### Example: AI Script

```python
import json

# Load CPU profile
with open('anito-debug/profiling/cpu_profile.json') as f:
    profile = json.load(f)

# Find bottlenecks (>10ms average)
bottlenecks = [
    scope for scope in profile['scopes']
    if scope['avg_time_ms'] > 10.0
]

# Report
for scope in bottlenecks:
    print(f"BOTTLENECK: {scope['name']} - {scope['avg_time_ms']:.2f}ms avg")
    print(f"  Suggestion: Optimize or parallelize this scope")
```

---

## Best Practices

1. ✅ **Profile early and often** - Catch performance regressions quickly
2. ✅ **Use scoped profiling** - Narrow down bottlenecks
3. ✅ **Profile in Release mode** - Debug builds are slower
4. ✅ **Compare profiles** - Before/after optimization
5. ✅ **Use Chrome Tracing** - Visual analysis is faster
6. ✅ **Enable memory profiling in Debug** - Catch leaks early
7. ✅ **Review logs regularly** - Warnings may indicate issues
8. ✅ **Analyze frame captures** - Visual artifacts may correlate with timing spikes

---

## Troubleshooting

### Issue: Profiling overhead too high

**Solution:** Disable profiling in Release builds or use lighter tracking modes

```cmake
# Only enable in Debug
if(CMAKE_BUILD_TYPE MATCHES "Release")
    # Don't define ANITO_ENABLE_PROFILING
endif()
```

### Issue: Chrome Tracing file too large

**Solution:** Reduce profiling duration or filter scopes

```cpp
// Only profile specific systems
if (shouldProfile("Rendering")) {
    ANITO_PROFILE_SCOPE("Rendering");
    render();
}
```

### Issue: Memory profiler reports false leaks

**Cause:** Static objects destroyed after profiler shutdown

**Solution:** Initialize profiler early, destroy late

```cpp
int main() {
    AnitoProfilerManager::initialize(...);

    // All other initialization...

    // Run game...

    // All cleanup BEFORE profiler destroy...

    AnitoProfilerManager::destroy(); // Last thing before exit
}
```

---

## Reference

- **CPU Profiler:** `src/Debug/AnitoPerformanceProfiler.h`
- **GPU Profiler:** `src/Debug/AnitoGPUProfiler.h`
- **Memory Profiler:** `src/Debug/AnitoMemoryProfiler.h`
- **Console Logger:** `src/Debug/AnitoConsoleLogger.h`
- **Frame Capture:** `src/Debug/AnitoFrameCapture.h`
- **Crash Reporter:** `src/Debug/AnitoCrashReporter.h`
- **Profiler Manager:** `src/Debug/AnitoProfilerManager.h`
