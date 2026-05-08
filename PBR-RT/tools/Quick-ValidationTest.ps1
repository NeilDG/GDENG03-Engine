# Quick-ValidationTest.ps1
# Simple validation test for Anito Engine

param(
    [int]$Duration = 15
)

Write-Host "======================================"  -ForegroundColor Cyan
Write-Host " ANITO ENGINE QUICK VALIDATION TEST"  -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# Step 1: Set runtime
Write-Host "[1/4] Setting runtime to $Duration seconds..." -ForegroundColor Yellow
& ".\tools\Set-EngineRuntime.ps1" -Seconds $Duration -CopyToBuild

# Step 2: Build
Write-Host "`n[2/4] Building engine (Release)..." -ForegroundColor Yellow
$buildStart = Get-Date
& ".\build.bat" build Release
$buildEnd = Get-Date
$buildTime = ($buildEnd - $buildStart).TotalSeconds

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build FAILED!" -ForegroundColor Red
    exit 1
}

Write-Host "Build completed in $([math]::Round($buildTime, 2))s" -ForegroundColor Green

# Step 3: Run
Write-Host "`n[3/4] Running engine for $Duration seconds..." -ForegroundColor Yellow
$runStart = Get-Date
& ".\build.bat" run Release
$runEnd = Get-Date
$runTime = ($runEnd - $runStart).TotalSeconds

Write-Host "Run completed in $([math]::Round($runTime, 2))s" -ForegroundColor Green

# Step 4: Analyze results
Write-Host "`n[4/4] Analyzing results..." -ForegroundColor Yellow

$frameCount = 0
$logCount = 0
$profileCount = 0

if (Test-Path "anito-debug\frames") {
    $frameCount = (Get-ChildItem "anito-debug\frames\*.png").Count
}

if (Test-Path "anito-debug\logs") {
    $logCount = (Get-ChildItem "anito-debug\logs\*.log").Count
}

if (Test-Path "anito-debug\profiling") {
    $profileCount = (Get-ChildItem "anito-debug\profiling\*.*").Count
}

# Display summary
Write-Host ""
Write-Host "======================================" -ForegroundColor Cyan
Write-Host "  VALIDATION SUMMARY" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host "Build Time: $([math]::Round($buildTime, 2))s" -ForegroundColor Green
Write-Host "Run Time: $([math]::Round($runTime, 2))s" -ForegroundColor Green
Write-Host "Frames Captured: $frameCount" -ForegroundColor Green
Write-Host "Log Files: $logCount" -ForegroundColor Green
Write-Host "Profiling Files: $profileCount" -ForegroundColor Green
Write-Host ""

if ($frameCount -gt 0) {
    Write-Host "Frame captures available in: anito-debug\frames" -ForegroundColor Cyan
}

if ($logCount -gt 0) {
    $latestLog = Get-ChildItem "anito-debug\logs\*.log" | Sort-Object LastWriteTime -Desc | Select-Object -First 1
    Write-Host "Latest log: $($latestLog.Name)" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Last 10 lines of log:" -ForegroundColor Yellow
    Get-Content $latestLog.FullName -Tail 10 | ForEach-Object { Write-Host "  $_" -ForegroundColor Gray }
}

Write-Host ""
Write-Host "Test complete!" -ForegroundColor Green

# Reset runtime
& ".\tools\Set-EngineRuntime.ps1" -Seconds 0 -CopyToBuild
Write-Host "Runtime reset to infinite." -ForegroundColor Gray
