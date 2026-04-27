# Quick Reference: Engine Runtime Control

## TL;DR

**Set runtime to 15 seconds:**
```powershell
.\tools\Set-EngineRuntime.ps1 -Seconds 15 -CopyToBuild
```

**Run engine:**
```powershell
.\Build.bat run
```

**Reset to infinite:**
```powershell
.\tools\Set-EngineRuntime.ps1 -Seconds 0 -CopyToBuild
```

---

## For AI Agents

### Standard Testing Workflow
```powershell
# 1. Set runtime
.\tools\Set-EngineRuntime.ps1 -Seconds 15 -CopyToBuild

# 2. Run engine
.\Build.bat run Release

# 3. Validate (engine auto-exits after 15 seconds)
Get-ChildItem anito-debug\frames | Measure-Object
Get-Content (Get-ChildItem anito-debug\logs | Sort-Object LastWriteTime -Descending | Select -First 1).FullName -Tail 20

# 4. Reset
.\tools\Set-EngineRuntime.ps1 -Seconds 0 -CopyToBuild
```

### Manual Config Edit
```powershell
(Get-Content engine_config.ini) -replace 'MaxRuntimeSeconds = 0', 'MaxRuntimeSeconds = 20' | Set-Content engine_config.ini
Copy-Item engine_config.ini build\bin\Release\engine_config.ini -Force
```

---

## Common Durations

| Duration | Use Case |
|----------|----------|
| 5 sec    | Quick smoke test |
| 10 sec   | Standard validation |
| 15 sec   | Frame capture (15-20 frames) |
| 30 sec   | Extended testing |
| 60 sec   | Performance profiling |
| 0 sec    | Interactive development |

---

## Files

- **Config**: `engine_config.ini`
- **Script**: `tools\Set-EngineRuntime.ps1`
- **Docs**: `docs\ENGINE_CONFIG.md`
- **Implementation**: `docs\RUNTIME_CONFIG_IMPLEMENTATION.md`
