@echo off
REM ========================================
REM Anito Engine - Simple Shader Compiler
REM ========================================

set SHADERC=external\bgfx\tools\shaderc.exe
set VARYING=assets\shaders\varying.def.sc
set INCLUDE=external\bgfx\src
set OUTDIR=assets\shaders\compiled

REM Check if shaderc exists
if not exist "%SHADERC%" (
    echo ERROR: shaderc.exe not found at %SHADERC%
    echo Please ensure the bgfx shader compiler is in the correct location.
    exit /b 1
)

REM Create output directory
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

echo ========================================
echo Compiling Anito Engine Shaders
echo ========================================
echo.
echo Shader Compiler: %SHADERC%
echo Output Directory: %OUTDIR%
echo.

echo Compiling vertex shader (vs_simple)...
"%SHADERC%" -f assets/shaders/vs_simple.sc -o %OUTDIR%/vs_simple.bin -i %INCLUDE% --varyingdef %VARYING% --platform windows --type vertex --profile s_5_0
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile vs_simple.sc
    exit /b 1
)
echo SUCCESS: vs_simple.bin

echo.
echo Compiling fragment shader (fs_simple)...
"%SHADERC%" -f assets/shaders/fs_simple.sc -o %OUTDIR%/fs_simple.bin -i %INCLUDE% --varyingdef %VARYING% --platform windows --type fragment --profile s_5_0
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to compile fs_simple.sc
    exit /b 1
)
echo SUCCESS: fs_simple.bin

echo.
echo ========================================
echo Shader compilation complete!
echo ========================================
echo.
echo Compiled shaders are in: %OUTDIR%
echo.
