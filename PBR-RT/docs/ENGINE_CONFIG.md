# Engine Configuration - Runtime Duration Control

## Overview

The Anito Engine now supports configurable runtime duration through the `engine_config.ini` file. This allows both users and AI agents to easily control how long the engine runs during testing and validation.

## Configuration File

The configuration file is located at: **`engine_config.ini`** (in the project root)

When you build the engine, this file is automatically copied to the build output directory alongside the executable.

## Setting Runtime Duration

### Option 1: Edit `engine_config.ini` directly

Open `engine_config.ini` in any text editor and modify the `MaxRuntimeSeconds` value:

```ini
[Runtime]
# Maximum runtime in seconds (0 = run indefinitely until window is closed)
MaxRuntimeSeconds = 10
```

**Examples:**
- `MaxRuntimeSeconds = 10` → Engine runs for 10 seconds then exits
- `MaxRuntimeSeconds = 30` → Engine runs for 30 seconds then exits
- `MaxRuntimeSeconds = 60` → Engine runs for 1 minute then exits
- `MaxRuntimeSeconds = 0` → Engine runs indefinitely (default behavior)

### Option 2: Edit the build directory copy

After building, you can also edit the configuration file in the build output directory:
- **Debug builds**: `build/bin/Debug/engine_config.ini`
- **Release builds**: `build/bin/Release/engine_config.ini`

Changes to these files will take effect immediately on the next run without rebuilding.

## Usage Examples

### For AI Agents

When validating changes or running automated tests:

```powershell
# 1. Set runtime to 15 seconds for validation
(Get-Content engine_config.ini) -replace 'MaxRuntimeSeconds = 0', 'MaxRuntimeSeconds = 15' | Set-Content engine_config.ini

# 2. Build (copies updated config)
.\Build.bat build Release

# 3. Run (will automatically exit after 15 seconds)
.\Build.bat run Release

# 4. Reset to infinite runtime
(Get-Content engine_config.ini) -replace 'MaxRuntimeSeconds = 15', 'MaxRuntimeSeconds = 0' | Set-Content engine_config.ini
```

### For Users

Manual testing scenarios:

```ini
# Quick smoke test (5 seconds)
MaxRuntimeSeconds = 5

# Frame capture session (20 seconds)
MaxRuntimeSeconds = 20

# Performance profiling (60 seconds)
MaxRuntimeSeconds = 60

# Normal interactive usage (infinite)
MaxRuntimeSeconds = 0
```

## How It Works

1. **Initialization**: When `AnitoEngine::initialize()` is called, the engine loads `engine_config.ini` and reads the `MaxRuntimeSeconds` value.

2. **Runtime Tracking**: During the main loop (`AnitoEngine::run()`), the engine tracks elapsed time using delta time accumulation.

3. **Automatic Exit**: When `elapsed time >= MaxRuntimeSeconds` (if > 0), the engine:
   - Prints a message: `"Runtime limit reached (X seconds). Exiting..."`
   - Gracefully shuts down all subsystems
   - Exits the main loop
   - Reports total runtime

4. **Infinite Mode**: When `MaxRuntimeSeconds = 0`, the engine runs until:
   - User closes the window (clicks X button)
   - User presses ESC key (if implemented)
   - Application crash (handled by crash reporter)

## Output

The engine logs runtime information:

```
[Engine] Loading configuration...
[Engine] Configuration loaded from: engine_config.ini
[Engine] Runtime limit: 15.0 seconds
[Engine] Engine will automatically exit after this duration.
...
[Engine] Starting main loop...
...
[Engine] Runtime limit reached (15 seconds). Exiting...
[Engine] Main loop ended.
[Engine] Total runtime: 15.023 seconds
```

## Benefits

✅ **Easy Testing**: Set short runtime for quick validation  
✅ **Automated Workflows**: AI agents can run tests without manual intervention  
✅ **Profiling**: Controlled duration for consistent performance measurements  
✅ **Frame Capture**: Capture specific number of frames in known timespan  
✅ **CI/CD Friendly**: Automated testing without hanging builds  

## Additional Configuration

The `engine_config.ini` file can be extended with additional settings:

```ini
[Debug]
VerboseLogging = true

[Rendering]
# Future: Add rendering quality settings
# AntiAliasing = MSAA4x
# ShadowQuality = High
```

See `src/Config/AnitoEngineConfig.h` for the configuration parser API.
