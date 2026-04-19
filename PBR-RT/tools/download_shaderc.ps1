# Download shaderc from bgfx releases
$shadercDir = "$PSScriptRoot\shaderc"
$shadercExe = "$shadercDir\shaderc.exe"

if (Test-Path $shadercExe) {
    Write-Host "shaderc already exists at: $shadercExe" -ForegroundColor Green
    exit 0
}

Write-Host "Downloading bgfx tools..." -ForegroundColor Cyan
New-Item -ItemType Directory -Force -Path $shadercDir | Out-Null

# Download from bgfx releases - use latest stable version
$url = "https://github.com/bkaradzic/bgfx/releases/download/v1.128.8755/bgfx-windows-x64.zip"
$zipFile = "$env:TEMP\bgfx-tools.zip"
$extractDir = "$env:TEMP\bgfx-tools"

try {
    Invoke-WebRequest -Uri $url -OutFile $zipFile -UseBasicParsing
    Write-Host "Extracting..." -ForegroundColor Cyan

    Expand-Archive -Path $zipFile -DestinationPath $extractDir -Force

    # Find shaderc.exe
    $shaderc = Get-ChildItem -Path $extractDir -Filter "shaderc.exe" -Recurse | Select-Object -First 1

    if ($shaderc) {
        Copy-Item $shaderc.FullName -Destination $shadercExe
        Write-Host "Successfully downloaded shaderc to: $shadercExe" -ForegroundColor Green
    } else {
        Write-Host "ERROR: Could not find shaderc.exe in the downloaded package" -ForegroundColor Red
        exit 1
    }

    # Cleanup
    Remove-Item $zipFile -Force -ErrorAction SilentlyContinue
    Remove-Item $extractDir -Recurse -Force -ErrorAction SilentlyContinue

} catch {
    Write-Host "ERROR: Failed to download shaderc: $_" -ForegroundColor Red
    exit 1
}
