# Quick Reference: Validation & Benchmarking

## One-Line Test Commands

```powershell
# Quick 15-second validation
.\tools\Quick-ValidationTest.ps1 -Duration 15

# Standard 30-second validation
.\tools\Quick-ValidationTest.ps1 -Duration 30

# Extended 60-second validation
.\tools\Quick-ValidationTest.ps1 -Duration 60
```

## Manual Build & Run

```powershell
# First time setup
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run (15 seconds then exit)
.\tools\Set-EngineRuntime.ps1 -Seconds 15 -CopyToBuild
.\build\bin\AnitoEngine.exe

# Reset to infinite runtime
.\tools\Set-EngineRuntime.ps1 -Seconds 0 -CopyToBuild
```

## View Results

```powershell
# Latest log
Get-Content (Get-ChildItem anito-debug\logs\*.log | Sort LastWriteTime -Desc | Select -First 1).FullName -Tail 20

# Frame captures
explorer anito-debug\frames

# CPU performance
Get-Content (Get-ChildItem anito-debug\profiling\cpu_*.json | Sort LastWriteTime -Desc | Select -First 1).FullName

# Quick FPS check
$cpu = Get-Content (Get-ChildItem anito-debug\profiling\cpu_*.json | Sort LastWriteTime -Desc | Select -First 1).FullName | ConvertFrom-Json; Write-Host "FPS: $($cpu.avg_fps)"
```

## Enable Benchmark Mode

```ini
# Edit engine_config.ini
[Benchmark]
EnableBenchmarkMode = true
BenchmarkDuration = Quick  # or Standard, Extended
```

Then run normally - benchmarks will execute automatically.

## Expected Performance (Release Build)

- **Target FPS:** 60
- **Current FPS:** ~48 (as of 2026-05-08)
- **Frame Time Budget:** 16.67ms (for 60 FPS)
- **Current Frame Time:** ~20.82ms

## Output Locations

- **Frames:** `anito-debug/frames/`
- **Logs:** `anito-debug/logs/`
- **Profiling:** `anito-debug/profiling/`
- **Benchmarks:** `anito-debug/benchmarks/` (when enabled)

## Validation Checklist

- [ ] Engine initializes without errors
- [ ] Window renders correctly
- [ ] Frame capture working (PNG files generated)
- [ ] Console logging operational
- [ ] CPU profiler collecting data
- [ ] Runtime configuration respected
- [ ] Clean shutdown after duration
- [ ] No crashes or exceptions
