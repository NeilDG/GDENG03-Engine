# Anito Engine - Dependency Setup Script
# Automatically clones all required external dependencies

Write-Host "========================================" -ForegroundColor Cyan
Write-Host " Anito Engine - Dependency Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$externalDir = "external"

# Create external directory if it doesn't exist
if (!(Test-Path $externalDir)) {
    Write-Host "Creating external directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path $externalDir | Out-Null
}

Set-Location $externalDir

# Function to clone or update repo
function Clone-Or-Update {
    param(
        [string]$name,
        [string]$url,
        [bool]$recursive = $false
    )

    Write-Host "Processing $name..." -ForegroundColor Green

    if (Test-Path $name) {
        Write-Host "  $name already exists, pulling latest..." -ForegroundColor Yellow
        Set-Location $name
        git pull
        if ($recursive) {
            git submodule update --init --recursive
        }
        Set-Location ..
    } else {
        Write-Host "  Cloning $name..." -ForegroundColor Yellow
        if ($recursive) {
            git clone --recursive $url
        } else {
            git clone $url
        }
    }

    Write-Host "  $name ready!" -ForegroundColor Green
    Write-Host ""
}

# Clone dependencies
Clone-Or-Update -name "bgfx" -url "https://github.com/bkaradzic/bgfx.git" -recursive $true
Clone-Or-Update -name "glfw" -url "https://github.com/glfw/glfw.git"
Clone-Or-Update -name "glm" -url "https://github.com/g-truc/glm.git"
Clone-Or-Update -name "bullet3" -url "https://github.com/bulletphysics/bullet3.git"
Clone-Or-Update -name "imgui" -url "https://github.com/ocornut/imgui.git"
Clone-Or-Update -name "ImGuizmo" -url "https://github.com/CedricGuillemet/ImGuizmo.git"

Set-Location ..

Write-Host "========================================" -ForegroundColor Cyan
Write-Host " All dependencies set up successfully!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Configure CMake: cmake --preset=default" -ForegroundColor White
Write-Host "  2. Build: cmake --build out/build/default" -ForegroundColor White
Write-Host "  3. Run: .\out\build\default\bin\AnitoEngine.exe" -ForegroundColor White
