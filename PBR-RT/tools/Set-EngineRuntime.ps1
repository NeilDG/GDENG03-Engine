# Set-EngineRuntime.ps1
# Helper script for AI agents and users to easily set engine runtime duration

param(
    [Parameter(Mandatory=$true)]
    [int]$Seconds,

    [Parameter(Mandatory=$false)]
    [switch]$CopyToBuild
)

$configFile = "engine_config.ini"

if (-not (Test-Path $configFile)) {
    Write-Error "Configuration file not found: $configFile"
    exit 1
}

Write-Host "Setting engine runtime to: $Seconds seconds" -ForegroundColor Cyan

if ($Seconds -eq 0) {
    Write-Host "  Mode: INFINITE (run until window closed)" -ForegroundColor Yellow
} else {
    Write-Host "  Mode: TIMED (automatic exit after $Seconds seconds)" -ForegroundColor Green
}

# Read the file, replace the value, write it back
$content = Get-Content $configFile
$newContent = $content -replace 'MaxRuntimeSeconds = \d+', "MaxRuntimeSeconds = $Seconds"
$newContent | Set-Content $configFile

Write-Host "Configuration updated successfully!" -ForegroundColor Green

# Optionally copy to build directories
if ($CopyToBuild) {
    Write-Host "`nCopying to build directories..." -ForegroundColor Cyan

    $buildPaths = @(
        "build\bin\Debug\engine_config.ini",
        "build\bin\Release\engine_config.ini"
    )

    foreach ($path in $buildPaths) {
        if (Test-Path (Split-Path $path -Parent)) {
            Copy-Item $configFile $path -Force
            Write-Host "  Copied to: $path" -ForegroundColor Gray
        }
    }

    Write-Host "Build directories updated!" -ForegroundColor Green
}

Write-Host "`nUsage examples:" -ForegroundColor Cyan
Write-Host "  .\Set-EngineRuntime.ps1 -Seconds 15         # Set to 15 seconds"
Write-Host "  .\Set-EngineRuntime.ps1 -Seconds 0          # Set to infinite"
Write-Host "  .\Set-EngineRuntime.ps1 -Seconds 30 -CopyToBuild  # Update build dirs too"
