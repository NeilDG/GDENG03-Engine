# Anito Engine - Build Configuration Validation Script
# Ensures Build.bat and CMakeLists.txt are synchronized

param(
    [switch]$Verbose = $false
)

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host " Anito Engine Configuration Validator" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Configuration expected values
$ExpectedConfig = @{
    CMakeMinVersion = "3.10"
    Generator = "Visual Studio 18 2026"
    Platform = "x64"
    CppStandard = "20"
    BuildDir = "build"
    OutputDir = "build/bin"
}

$ValidationErrors = @()
$ValidationWarnings = @()

# Function to validate CMakeLists.txt
function Test-CMakeListsConfiguration {
    Write-Host "Checking CMakeLists.txt..." -ForegroundColor Yellow

    if (-not (Test-Path "CMakeLists.txt")) {
        $script:ValidationErrors += "CMakeLists.txt not found"
        return
    }

    $cmakeContent = Get-Content "CMakeLists.txt" -Raw

    # Check CMake minimum version
    if ($cmakeContent -match "cmake_minimum_required\(VERSION\s+(\d+\.\d+)") {
        $cmakeMinVersion = $matches[1]
        if ($cmakeMinVersion -eq $ExpectedConfig.CMakeMinVersion) {
            Write-Host "  [OK] CMake min version: $cmakeMinVersion" -ForegroundColor Green
        } else {
            $script:ValidationWarnings += "CMake min version mismatch: Expected $($ExpectedConfig.CMakeMinVersion), Found $cmakeMinVersion"
        }
    } else {
        $script:ValidationErrors += "CMake minimum version not specified in CMakeLists.txt"
    }

    # Check C++ standard
    if ($cmakeContent -match "CMAKE_CXX_STANDARD\s+(\d+)") {
        $cppStandard = $matches[1]
        if ($cppStandard -eq $ExpectedConfig.CppStandard) {
            Write-Host "  [OK] C++ Standard: C++$cppStandard" -ForegroundColor Green
        } else {
            $script:ValidationWarnings += "C++ Standard mismatch: Expected C++$($ExpectedConfig.CppStandard), Found C++$cppStandard"
        }
    } else {
        $script:ValidationErrors += "C++ Standard not specified in CMakeLists.txt"
    }

    # Check output directories
    if ($cmakeContent -match "CMAKE_RUNTIME_OUTPUT_DIRECTORY") {
        Write-Host "  [OK] Output directory configuration found" -ForegroundColor Green
    } else {
        $script:ValidationWarnings += "CMAKE_RUNTIME_OUTPUT_DIRECTORY not set"
    }
}

# Function to validate Build.bat
function Test-BuildBatConfiguration {
    Write-Host ""
    Write-Host "Checking Build.bat..." -ForegroundColor Yellow

    if (-not (Test-Path "Build.bat")) {
        $script:ValidationErrors += "Build.bat not found"
        return
    }

    $buildBatContent = Get-Content "Build.bat" -Raw

    # Check for generator specification
    if ($buildBatContent -match "Visual Studio 18 2026") {
        Write-Host "  [OK] Generator: Visual Studio 18 2026" -ForegroundColor Green
    } else {
        $script:ValidationErrors += "Build.bat does not specify Visual Studio 18 2026 generator"
    }

    # Check for platform specification
    if ($buildBatContent -match "-A x64") {
        Write-Host "  [OK] Platform: x64" -ForegroundColor Green
    } else {
        $script:ValidationWarnings += "Build.bat does not explicitly set platform to x64"
    }

    # Check for C++ standard
    if ($buildBatContent -match "CMAKE_CXX_STANDARD=20") {
        Write-Host "  [OK] C++ Standard: C++20" -ForegroundColor Green
    } else {
        $script:ValidationWarnings += "Build.bat does not explicitly set C++ standard"
    }

    # Check for CMake version validation
    if ($buildBatContent -match "cmake --version.*findstr") {
        Write-Host "  [OK] CMake version validation present" -ForegroundColor Green
    } else {
        $script:ValidationWarnings += "Build.bat does not validate CMake version"
    }
}

# Function to validate current build directory
function Test-BuildDirectoryConfiguration {
    Write-Host ""
    Write-Host "Checking build directory..." -ForegroundColor Yellow

    if (-not (Test-Path "build/CMakeCache.txt")) {
        Write-Host "  [INFO] Build directory not configured yet (run 'Build.bat setup')" -ForegroundColor Gray
        return
    }

    $cacheContent = Get-Content "build/CMakeCache.txt" -Raw

    # Check generator
    if ($cacheContent -match "CMAKE_GENERATOR:INTERNAL=(.+)") {
        $actualGenerator = $matches[1].Trim()
        if ($actualGenerator -eq $ExpectedConfig.Generator) {
            Write-Host "  [OK] Configured generator: $actualGenerator" -ForegroundColor Green
        } else {
            $script:ValidationErrors += "Build directory generator mismatch: Expected '$($ExpectedConfig.Generator)', Found '$actualGenerator'"
        }
    }

    # Check platform
    if ($cacheContent -match "CMAKE_GENERATOR_PLATFORM:INTERNAL=(.*)") {
        $actualPlatform = $matches[1]
        if ([string]::IsNullOrWhiteSpace($actualPlatform)) {
            $actualPlatform = "default (x64)"
        }
        Write-Host "  [OK] Platform: $actualPlatform" -ForegroundColor Green
    }

    # Check C++ standard
    if ($cacheContent -match "CMAKE_CXX_STANDARD:STRING=(\d+)") {
        $actualCppStandard = $matches[1]
        if ($actualCppStandard -eq $ExpectedConfig.CppStandard) {
            Write-Host "  [OK] C++ Standard: C++$actualCppStandard" -ForegroundColor Green
        } else {
            $script:ValidationWarnings += "Build C++ standard mismatch: Expected C++$($ExpectedConfig.CppStandard), Found C++$actualCppStandard"
        }
    }
}

# Function to check Visual Studio installation
function Test-VisualStudioInstallation {
    Write-Host ""
    Write-Host "Checking Visual Studio 2026..." -ForegroundColor Yellow

    $vsPath = "C:\Program Files\Microsoft Visual Studio"
    $year = "2026"
    # Also check "18" folder for VS 2026 preview versions
    $yearFolders = @("2026", "18")
    $editions = @("Community", "Professional", "Enterprise", "Preview")
    $found = $false

    foreach ($yearFolder in $yearFolders) {
        foreach ($edition in $editions) {
            $devenvPath = Join-Path $vsPath "$yearFolder\$edition\Common7\IDE\devenv.exe"
            if (Test-Path $devenvPath) {
                Write-Host "  [OK] Visual Studio 2026 $edition found" -ForegroundColor Green
                $found = $true
                break
            }
        }
        if ($found) { break }
    }

    if (-not $found) {
        $script:ValidationWarnings += "Visual Studio 2026 not found at standard location (CMake may still find it via vswhere)"
    }
}

# Run all validations
Test-CMakeListsConfiguration
Test-BuildBatConfiguration
Test-BuildDirectoryConfiguration
Test-VisualStudioInstallation

# Report results
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host " Validation Results" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

if ($ValidationErrors.Count -eq 0 -and $ValidationWarnings.Count -eq 0) {
    Write-Host ""
    Write-Host "[PASS] All checks passed!" -ForegroundColor Green
    Write-Host "Build.bat and CMakeLists.txt are synchronized." -ForegroundColor Green
    Write-Host ""
    exit 0
} else {
    if ($ValidationErrors.Count -gt 0) {
        Write-Host ""
        Write-Host "[ERROR] ERRORS FOUND:" -ForegroundColor Red
        foreach ($err in $ValidationErrors) {
            Write-Host "  - $err" -ForegroundColor Red
        }
    }

    if ($ValidationWarnings.Count -gt 0) {
        Write-Host ""
        Write-Host "[WARNING] WARNINGS:" -ForegroundColor Yellow
        foreach ($warn in $ValidationWarnings) {
            Write-Host "  - $warn" -ForegroundColor Yellow
        }
    }

    Write-Host ""
    if ($ValidationErrors.Count -gt 0) {
        Write-Host "Configuration validation FAILED. Please fix errors above." -ForegroundColor Red
        exit 1
    } else {
        Write-Host "Configuration validation passed with warnings." -ForegroundColor Yellow
        exit 0
    }
}
