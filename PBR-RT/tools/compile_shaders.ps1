# Shader compilation script for Anito Engine
# Uses shaderc from bgfx to compile .sc files to platform-specific shaders

param(
    [string]$ShadercPath = "",
    [string]$ShaderSrcDir = "assets\shaders",
    [string]$ShaderOutDir = "assets\shaders\compiled",
    [string]$BgfxIncludeDir = "external\bgfx\src"
)

# Auto-detect shaderc if not specified
if ([string]::IsNullOrEmpty($ShadercPath)) {
    $possiblePaths = @(
        "external\bgfx\tools\shaderc.exe",
        "tools\shaderc\shaderc.exe",
        "external\bgfx\.build\win64_vs2022\bin\shadercRelease.exe",
        "external\bgfx\.build\win64_vs2019\bin\shadercRelease.exe",
        "external\bgfx\.build\win32_vs2022\bin\shadercRelease.exe"
    )

    foreach ($path in $possiblePaths) {
        if (Test-Path $path) {
            $ShadercPath = $path
            break
        }
    }
}

# Create output directory if it doesn't exist
if (-not (Test-Path $ShaderOutDir)) {
    New-Item -ItemType Directory -Force -Path $ShaderOutDir | Out-Null
}

# Detect platform
$platform = "windows"
$profile = "s_5_0"  # Shader Model 5.0 for DirectX 11/12

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Anito Engine Shader Compiler" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if shaderc exists, if not try to download it
if (-not (Test-Path $ShadercPath)) {
    Write-Host "shaderc not found, attempting to build from bgfx source..." -ForegroundColor Yellow
    $buildScript = Join-Path $PSScriptRoot "build_shaderc.bat"

    if (Test-Path $buildScript) {
        & $buildScript

        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: Failed to build shaderc" -ForegroundColor Red
            Write-Host "Please run: tools\build_shaderc.bat" -ForegroundColor Yellow
            exit 1
        }

        # Re-check for shaderc after building
        if (Test-Path "tools\shaderc\shaderc.exe") {
            $ShadercPath = "tools\shaderc\shaderc.exe"
        }
    } else {
        Write-Host "ERROR: shaderc not found and build script missing" -ForegroundColor Red
        Write-Host "" -ForegroundColor Yellow
        Write-Host "To build shaderc from bgfx source:" -ForegroundColor Yellow
        Write-Host "  Run: tools\build_shaderc.bat" -ForegroundColor White
        Write-Host "" -ForegroundColor Yellow
        Write-Host "This will:" -ForegroundColor Yellow
        Write-Host "  1. Generate Visual Studio projects using GENie" -ForegroundColor White
        Write-Host "  2. Build shaderc in Release mode" -ForegroundColor White
        Write-Host "  3. Copy shaderc.exe to tools\shaderc\" -ForegroundColor White
        exit 1
    }
}

Write-Host "Shaderc: $ShadercPath" -ForegroundColor Green
Write-Host "Source:  $ShaderSrcDir" -ForegroundColor Green
Write-Host "Output:  $ShaderOutDir" -ForegroundColor Green
Write-Host ""

# Function to compile shader
function Compile-Shader {
    param(
        [string]$ShaderFile,
        [string]$Type,
        [string]$OutputName,
        [string]$VaryingDefOverride = ""
    )

    if ([string]::IsNullOrEmpty($VaryingDefOverride)) {
        $varyingDef = Join-Path $ShaderSrcDir "varying.def.sc"
    } else {
        $varyingDef = $VaryingDefOverride
    }

    $outputFile = Join-Path $ShaderOutDir "$OutputName.bin"

    Write-Host "Compiling: $ShaderFile -> $OutputName.bin ($Type)" -ForegroundColor Cyan

    $args = @(
        "-f", $ShaderFile,
        "-o", $outputFile,
        "-i", $BgfxIncludeDir,
        "--varyingdef", $varyingDef,
        "--platform", $platform,
        "--type", $Type,
        "--profile", $profile
    )

    & $ShadercPath $args

    if ($LASTEXITCODE -eq 0) {
        Write-Host "  SUCCESS" -ForegroundColor Green
    } else {
        Write-Host "  FAILED" -ForegroundColor Red
    }
}

# Compile shaders
Write-Host "Compiling shaders..." -ForegroundColor Yellow
Write-Host ""

# Simple PBR shader
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "vs_simple.sc") -Type "vertex" -OutputName "vs_simple"
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "fs_simple.sc") -Type "fragment" -OutputName "fs_simple"

# Skybox shader
$skyboxVarying = Join-Path $ShaderSrcDir "varying_skybox.def.sc"
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "vs_skybox.sc") -Type "vertex" -OutputName "vs_skybox" -VaryingDefOverride $skyboxVarying
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "fs_skybox.sc") -Type "fragment" -OutputName "fs_skybox" -VaryingDefOverride $skyboxVarying

# Equirectangular to cubemap conversion shader
$equirectVarying = Join-Path $ShaderSrcDir "varying_equirect_to_cubemap.def.sc"
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "vs_equirect_to_cubemap.sc") -Type "vertex" -OutputName "vs_equirect_to_cubemap" -VaryingDefOverride $equirectVarying
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "fs_equirect_to_cubemap.sc") -Type "fragment" -OutputName "fs_equirect_to_cubemap" -VaryingDefOverride $equirectVarying

# Irradiance convolution shader (for diffuse IBL)
$irradianceVarying = Join-Path $ShaderSrcDir "varying_irradiance_convolution.def.sc"
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "vs_irradiance_convolution.sc") -Type "vertex" -OutputName "vs_irradiance_convolution" -VaryingDefOverride $irradianceVarying
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "fs_irradiance_convolution.sc") -Type "fragment" -OutputName "fs_irradiance_convolution" -VaryingDefOverride $irradianceVarying

# Prefilter environment map shader (for specular IBL)
$prefilterVarying = Join-Path $ShaderSrcDir "varying_prefilter_envmap.def.sc"
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "vs_prefilter_envmap.sc") -Type "vertex" -OutputName "vs_prefilter_envmap" -VaryingDefOverride $prefilterVarying
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "fs_prefilter_envmap.sc") -Type "fragment" -OutputName "fs_prefilter_envmap" -VaryingDefOverride $prefilterVarying

# G-Buffer shaders (deferred rendering - geometry pass)
$gbufferVarying = Join-Path $ShaderSrcDir "varying_gbuffer.def.sc"
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "vs_gbuffer.sc") -Type "vertex" -OutputName "vs_gbuffer" -VaryingDefOverride $gbufferVarying
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "fs_gbuffer.sc") -Type "fragment" -OutputName "fs_gbuffer" -VaryingDefOverride $gbufferVarying

# Deferred lighting shaders (deferred rendering - lighting pass)
$deferredLightVarying = Join-Path $ShaderSrcDir "varying_deferred_light.def.sc"
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "vs_deferred_light.sc") -Type "vertex" -OutputName "vs_deferred_light" -VaryingDefOverride $deferredLightVarying
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "fs_deferred_light.sc") -Type "fragment" -OutputName "fs_deferred_light" -VaryingDefOverride $deferredLightVarying

# G-Buffer debug visualization shaders (Step 10)
$gbufferDebugVarying = Join-Path $ShaderSrcDir "varying_gbuffer_debug.def.sc"
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "vs_gbuffer_debug.sc") -Type "vertex" -OutputName "vs_gbuffer_debug" -VaryingDefOverride $gbufferDebugVarying
Compile-Shader -ShaderFile (Join-Path $ShaderSrcDir "fs_gbuffer_debug.sc") -Type "fragment" -OutputName "fs_gbuffer_debug" -VaryingDefOverride $gbufferDebugVarying

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Shader compilation complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
