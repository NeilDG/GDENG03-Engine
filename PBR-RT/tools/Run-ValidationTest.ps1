# Run-ValidationTest.ps1
# Comprehensive validation and benchmarking script for Anito Engine
# Project Anito - DOST-PCIEERD Funded Initiative

param(
    [Parameter(Mandatory=$false)]
    [ValidateSet("Quick", "Standard", "Full")]
    [string]$TestMode = "Quick",

    [Parameter(Mandatory=$false)]
    [switch]$Benchmark,

    [Parameter(Mandatory=$false)]
    [string]$BuildConfig = "Release"
)

$ErrorActionPreference = "Stop"

Write-Host "======================================" -ForegroundColor Cyan
Write-Host "  ANITO ENGINE VALIDATION TEST" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""

# Define test parameters
$testDurations = @{
    "Quick" = 15
    "Standard" = 30
    "Full" = 60
}

$testDuration = $testDurations[$TestMode]
$timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"

Write-Host "Test Mode: $TestMode ($testDuration seconds)" -ForegroundColor Green
Write-Host "Build Config: $BuildConfig" -ForegroundColor Green
$benchmarkStatus = if ($Benchmark) { 'ENABLED' } else { 'DISABLED' }
Write-Host "Benchmark Mode: $benchmarkStatus" -ForegroundColor Green
Write-Host "Timestamp: $timestamp" -ForegroundColor Gray
Write-Host ""

# Step 1: Configure engine for test
Write-Host "[1/6] Configuring engine runtime..." -ForegroundColor Yellow

if ($Benchmark) {
    # Enable benchmark mode
    $configContent = Get-Content "engine_config.ini"
    $configContent = $configContent -replace 'EnableBenchmarkMode = \w+', 'EnableBenchmarkMode = true'
    $configContent = $configContent -replace 'BenchmarkDuration = \w+', "BenchmarkDuration = $TestMode"
    $configContent | Set-Content "engine_config.ini"
    Write-Host "  ✓ Benchmark mode ENABLED" -ForegroundColor Green
} else {
    # Set runtime duration
    & ".\tools\Set-EngineRuntime.ps1" -Seconds $testDuration -CopyToBuild
    Write-Host "  ✓ Runtime set to $testDuration seconds" -ForegroundColor Green
}

# Step 2: Clean previous test results
Write-Host "`n[2/6] Cleaning previous test results..." -ForegroundColor Yellow

$debugDir = "anito-debug"
if (Test-Path $debugDir) {
    $backupDir = "anito-debug-backup-$timestamp"
    Write-Host "  Creating backup: $backupDir" -ForegroundColor Gray
    Move-Item $debugDir $backupDir -Force
}

New-Item -ItemType Directory -Path $debugDir -Force | Out-Null
Write-Host "  ✓ Clean test environment ready" -ForegroundColor Green

# Step 3: Build the engine
Write-Host "`n[3/6] Building engine ($BuildConfig)..." -ForegroundColor Yellow

$buildStart = Get-Date
& ".\build.bat" build $BuildConfig

if ($LASTEXITCODE -ne 0) {
    Write-Host "  ✗ Build FAILED!" -ForegroundColor Red
    exit 1
}

$buildEnd = Get-Date
$buildTime = ($buildEnd - $buildStart).TotalSeconds
Write-Host "  ✓ Build completed in $([math]::Round($buildTime, 2)) seconds" -ForegroundColor Green

# Step 4: Run the engine
Write-Host "`n[4/6] Running validation test..." -ForegroundColor Yellow
Write-Host "  Press Ctrl+C to stop early (if needed)" -ForegroundColor Gray
Write-Host ""

$runStart = Get-Date
& ".\build.bat" run $BuildConfig

$runEnd = Get-Date
$runTime = ($runEnd - $runStart).TotalSeconds

Write-Host ""
Write-Host "  ✓ Test run completed in $([math]::Round($runTime, 2)) seconds" -ForegroundColor Green

# Step 5: Analyze results
Write-Host "`n[5/6] Analyzing results..." -ForegroundColor Yellow

$results = @{
    timestamp = $timestamp
    testMode = $TestMode
    buildConfig = $BuildConfig
    benchmarkMode = $Benchmark.IsPresent
    buildTimeSeconds = [math]::Round($buildTime, 2)
    runTimeSeconds = [math]::Round($runTime, 2)
    logs = @()
    frames = @()
    profiling = @()
    benchmarks = @()
}

# Check logs
if (Test-Path "$debugDir\logs") {
    $logFiles = Get-ChildItem "$debugDir\logs\*.log"
    $results.logs = $logFiles | ForEach-Object { $_.Name }
    Write-Host "  ✓ Found $($logFiles.Count) log file(s)" -ForegroundColor Green
}

# Check frames
if (Test-Path "$debugDir\frames") {
    $frameFiles = Get-ChildItem "$debugDir\frames\*.png"
    $results.frames = $frameFiles | ForEach-Object { $_.Name }
    Write-Host "  ✓ Captured $($frameFiles.Count) frame(s)" -ForegroundColor Green
}

# Check profiling data
if (Test-Path "$debugDir\profiling") {
    $profilingFiles = Get-ChildItem "$debugDir\profiling\*.*"
    $results.profiling = $profilingFiles | ForEach-Object { $_.Name }
    Write-Host "  ✓ Generated $($profilingFiles.Count) profiling file(s)" -ForegroundColor Green
}

# Check benchmark results
if ($Benchmark) {
    if (Test-Path "$debugDir\benchmark_results.json") {
        $benchmarkData = Get-Content "$debugDir\benchmark_results.json" | ConvertFrom-Json
        $results.benchmarks = $benchmarkData
        Write-Host "  ✓ Benchmark results available" -ForegroundColor Green
    }
}

# Step 6: Generate report
Write-Host "`n[6/6] Generating validation report..." -ForegroundColor Yellow

$reportPath = "validation_report_$timestamp.json"
$results | ConvertTo-Json -Depth 10 | Set-Content $reportPath

Write-Host "  ✓ Report saved to: $reportPath" -ForegroundColor Green

# Display summary
Write-Host ""
Write-Host "======================================" -ForegroundColor Cyan
Write-Host "  VALIDATION TEST SUMMARY" -ForegroundColor Cyan
Write-Host "======================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Build Time: " -NoNewline
Write-Host "$([math]::Round($buildTime, 2))s" -ForegroundColor Green
Write-Host "Run Time: " -NoNewline
Write-Host "$([math]::Round($runTime, 2))s" -ForegroundColor Green
Write-Host "Frames Captured: " -NoNewline
Write-Host "$($results.frames.Count)" -ForegroundColor Green
Write-Host "Log Files: " -NoNewline
Write-Host "$($results.logs.Count)" -ForegroundColor Green
Write-Host "Profiling Files: " -NoNewline
Write-Host "$($results.profiling.Count)" -ForegroundColor Green

if ($Benchmark -and $results.benchmarks) {
    Write-Host ""
    Write-Host "BENCHMARK RESULTS:" -ForegroundColor Yellow
    # Display key benchmark metrics
    Write-Host "  (See $debugDir\benchmark_results.json for details)" -ForegroundColor Gray
}

Write-Host ""
Write-Host "✓ VALIDATION TEST COMPLETE!" -ForegroundColor Green
Write-Host ""
Write-Host "Next Steps:" -ForegroundColor Cyan
Write-Host "  1. Review logs: Get-Content $debugDir\logs\*.log" -ForegroundColor Gray
Write-Host "  2. View frames: explorer $debugDir\frames" -ForegroundColor Gray
Write-Host "  3. Check profiling: Get-Content $debugDir\profiling\*.json" -ForegroundColor Gray
if ($Benchmark) {
    Write-Host "  4. Analyze benchmarks: Get-Content $debugDir\benchmark_results.json" -ForegroundColor Gray
}
Write-Host ""

# Reset config
if (-not $Benchmark) {
    & ".\tools\Set-EngineRuntime.ps1" -Seconds 0 -CopyToBuild
    Write-Host "Runtime configuration reset to infinite." -ForegroundColor Gray
}
