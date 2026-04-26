# Visual Workflow Guide: Runtime Configuration

## Quick Start Visual Guide

```
┌─────────────────────────────────────────────────────────────┐
│  STEP 1: Set Runtime Duration                              │
└─────────────────────────────────────────────────────────────┘

PowerShell Command:
┌────────────────────────────────────────────────────────────┐
│ .\tools\Set-EngineRuntime.ps1 -Seconds 15 -CopyToBuild    │
└────────────────────────────────────────────────────────────┘
                         │
                         ▼
         ┌───────────────────────────────┐
         │  Updates engine_config.ini    │
         │  MaxRuntimeSeconds = 15       │
         └───────────────────────────────┘
                         │
                         ▼
    ┌──────────────────────────────────────────┐
    │  Copies to build directories:            │
    │  • build/bin/Debug/engine_config.ini     │
    │  • build/bin/Release/engine_config.ini   │
    └──────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  STEP 2: Build & Run Engine                                │
└─────────────────────────────────────────────────────────────┘

PowerShell Command:
┌────────────────────────────────────────────────────────────┐
│ .\Build.bat run Release                                    │
└────────────────────────────────────────────────────────────┘
                         │
                         ▼
         ┌───────────────────────────────┐
         │  Engine Initializes           │
         │  Loads engine_config.ini      │
         │  Reads MaxRuntimeSeconds      │
         └───────────────────────────────┘
                         │
                         ▼
         ┌───────────────────────────────┐
         │  Main Loop Starts             │
         │  Tracks elapsed time          │
         │  Renders frames               │
         └───────────────────────────────┘
                         │
                         ▼
         ┌───────────────────────────────┐
         │  15 Seconds Elapsed           │
         │  Auto-Exit Triggered          │
         └───────────────────────────────┘
                         │
                         ▼
         ┌───────────────────────────────┐
         │  Clean Shutdown               │
         │  Exports profiling data       │
         │  Reports total runtime        │
         └───────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  STEP 3: Validate Results                                  │
└─────────────────────────────────────────────────────────────┘

Check frame captures:
┌────────────────────────────────────────────────────────────┐
│ Get-ChildItem anito-debug\frames | Measure-Object         │
└────────────────────────────────────────────────────────────┘
                         │
                         ▼
              ┌──────────────────┐
              │  Count: 15-20    │
              │  frames captured │
              └──────────────────┘

Check logs:
┌────────────────────────────────────────────────────────────┐
│ Get-Content anito-debug\logs\*.log -Tail 20               │
└────────────────────────────────────────────────────────────┘
                         │
                         ▼
              ┌──────────────────┐
              │  Verify:         │
              │  • No errors     │
              │  • Clean exit    │
              │  • Runtime: ~15s │
              └──────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  STEP 4: Reset to Infinite (Optional)                      │
└─────────────────────────────────────────────────────────────┘

PowerShell Command:
┌────────────────────────────────────────────────────────────┐
│ .\tools\Set-EngineRuntime.ps1 -Seconds 0 -CopyToBuild     │
└────────────────────────────────────────────────────────────┘
                         │
                         ▼
         ┌───────────────────────────────┐
         │  MaxRuntimeSeconds = 0        │
         │  (Normal behavior restored)   │
         └───────────────────────────────┘
```

---

## Configuration File Structure

```ini
┌─────────────────────────────────────────────────────────────┐
│  engine_config.ini                                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  [Runtime]                                                  │
│  # Maximum runtime in seconds                              │
│  # 0 = run indefinitely (default)                          │
│  # Any positive value = auto-exit after X seconds          │
│  MaxRuntimeSeconds = 0  ◄── Change this value              │
│                                                             │
│  [Debug]                                                    │
│  VerboseLogging = true                                      │
│  FrameCaptureInterval = 1.0                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Console Output Flow

```
┌─────────────────────────────────────────────────────────────┐
│  INITIALIZATION PHASE                                       │
└─────────────────────────────────────────────────────────────┘

[Engine] Loading configuration...
[Engine] Configuration loaded from: engine_config.ini
[Engine] Runtime limit: 15.0 seconds  ◄── Confirms setting
[Engine] Engine will automatically exit after this duration.
[Engine] Initializing profiling system...
[Engine] Profiling system initialized - Output: anito-debug/

         ┌───────────────────────────────┐
         │  Subsystems Initialize        │
         │  • Window                     │
         │  • Renderer                   │
         │  • Input                      │
         │  • Game Objects               │
         │  • Frame Capture              │
         └───────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  RUNTIME PHASE                                              │
└─────────────────────────────────────────────────────────────┘

[Engine] Starting main loop...
[AnitoRenderer] Frame 1
[FrameCapture] Frame 1 saved
[AnitoRenderer] Frame 2
[FrameCapture] Frame 2 saved
...
[AnitoRenderer] Frame 15
[FrameCapture] Frame 15 saved

         ┌───────────────────────────────┐
         │  Elapsed: 15.0 seconds        │
         │  Limit Reached!               │
         └───────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│  SHUTDOWN PHASE                                             │
└─────────────────────────────────────────────────────────────┘

[Engine] Runtime limit reached (15 seconds). Exiting... ◄── Auto-exit
[Engine] Main loop ended.
[Engine] Total runtime: 15.0052 seconds  ◄── Accurate timing
[Engine] Shutting down Anito Engine...
[Engine] Exporting profiling data...
[ProfilerManager] All profiles exported successfully
[Engine] Shutdown complete.
```

---

## Helper Script Output

```
┌─────────────────────────────────────────────────────────────┐
│  Running: Set-EngineRuntime.ps1 -Seconds 15 -CopyToBuild   │
└─────────────────────────────────────────────────────────────┘

Setting engine runtime to: 15 seconds
  Mode: TIMED (automatic exit after 15 seconds)

Configuration updated successfully!

Copying to build directories...
  Copied to: build\bin\Debug\engine_config.ini
  Copied to: build\bin\Release\engine_config.ini
Build directories updated!

Usage examples:
  .\Set-EngineRuntime.ps1 -Seconds 15         # Set to 15 seconds
  .\Set-EngineRuntime.ps1 -Seconds 0          # Set to infinite
  .\Set-EngineRuntime.ps1 -Seconds 30 -CopyToBuild  # Update build dirs
```

---

## Common Duration Presets

```
┌──────────┬─────────────────────────────────────────────────┐
│ Duration │ Use Case                                        │
├──────────┼─────────────────────────────────────────────────┤
│  5 sec   │ Quick smoke test                                │
│          │ • Verify engine starts                          │
│          │ • Check for immediate crashes                   │
├──────────┼─────────────────────────────────────────────────┤
│ 10 sec   │ Standard validation                             │
│          │ • AI agent feedback loop                        │
│          │ • Quick feature testing                         │
├──────────┼─────────────────────────────────────────────────┤
│ 15 sec   │ Frame capture session                           │
│          │ • Capture 15-20 frames                          │
│          │ • Visual regression testing                     │
├──────────┼─────────────────────────────────────────────────┤
│ 30 sec   │ Extended testing                                │
│          │ • Scene transition testing                      │
│          │ • Stability validation                          │
├──────────┼─────────────────────────────────────────────────┤
│ 60 sec   │ Performance profiling                           │
│          │ • Collect detailed metrics                      │
│          │ • Memory leak detection                         │
├──────────┼─────────────────────────────────────────────────┤
│  0 sec   │ Interactive development (DEFAULT)               │
│          │ • Manual testing                                │
│          │ • Debugging                                     │
│          │ • Normal usage                                  │
└──────────┴─────────────────────────────────────────────────┘
```

---

## File Locations

```
Project Root (C:\...\PBR-RT\)
│
├── engine_config.ini  ◄── Primary config (edit this)
│
├── tools\
│   └── Set-EngineRuntime.ps1  ◄── Helper script
│
├── build\
│   └── bin\
│       ├── Debug\
│       │   └── engine_config.ini  ◄── Auto-copied on build
│       └── Release\
│           └── engine_config.ini  ◄── Auto-copied on build
│
├── src\
│   ├── Config\
│   │   └── AnitoEngineConfig.h  ◄── Parser implementation
│   ├── AnitoEngine.h  ◄── Runtime tracking
│   └── AnitoEngine.cpp  ◄── Main logic
│
└── docs\
    ├── ENGINE_CONFIG.md  ◄── Full documentation
    ├── RUNTIME_CONFIG_IMPLEMENTATION.md  ◄── Implementation details
    └── RUNTIME_CONFIG_QUICKREF.md  ◄── Quick reference
```

---

## Decision Tree: Which Method to Use?

```
                    Need to set runtime?
                           │
                           │
        ┌──────────────────┴──────────────────┐
        │                                     │
        ▼                                     ▼
   One-time quick test?            Frequent testing / AI agent?
        │                                     │
        │                                     │
        ▼                                     ▼
   Edit config directly              Use helper script
   ┌─────────────────┐              ┌──────────────────────┐
   │ notepad          │              │ .\tools\             │
   │ engine_config.   │              │ Set-EngineRuntime.   │
   │ ini              │              │ ps1 -Seconds X       │
   │                  │              │ -CopyToBuild         │
   │ Change:          │              └──────────────────────┘
   │ MaxRuntimeSec    │                       │
   │ = 15             │                       │
   └─────────────────┘                       │
           │                                  │
           └──────────────┬───────────────────┘
                          │
                          ▼
                    .\Build.bat run
                          │
                          ▼
                  Engine runs for X seconds
                          │
                          ▼
                    Auto-exits cleanly
```

---

## Troubleshooting

```
┌─────────────────────────────────────────────────────────────┐
│  Problem: Engine still runs indefinitely                    │
└─────────────────────────────────────────────────────────────┘
Solution:
1. Check config file in RUNNING directory
   Get-Content build\bin\Release\engine_config.ini

2. Verify MaxRuntimeSeconds value
   Should be: MaxRuntimeSeconds = 15 (not 0)

3. Copy updated config
   Copy-Item engine_config.ini build\bin\Release\ -Force

4. Run again
   .\Build.bat run

┌─────────────────────────────────────────────────────────────┐
│  Problem: Config file not found                             │
└─────────────────────────────────────────────────────────────┘
Solution:
1. Rebuild project (auto-copies config)
   .\Build.bat build

2. Or manually copy
   Copy-Item engine_config.ini build\bin\Release\ -Force

┌─────────────────────────────────────────────────────────────┐
│  Problem: Script won't run (execution policy)               │
└─────────────────────────────────────────────────────────────┘
Solution:
1. Use bypass flag
   powershell -ExecutionPolicy Bypass -File tools\Set-EngineRuntime.ps1 -Seconds 15

2. Or set execution policy
   Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
```
