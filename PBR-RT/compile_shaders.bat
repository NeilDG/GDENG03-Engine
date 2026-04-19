@echo off
echo ========================================
echo Anito Engine - Manual Shader Compiler
echo ========================================
echo.

REM Check if shaderc exists
set SHADERC=external\bgfx\tools\shaderc.exe
if not exist "%SHADERC%" (
    set SHADERC=tools\shaderc\shaderc.exe
    if not exist "%SHADERC%" (
        echo ERROR: shaderc.exe not found!
        echo.
        echo Checked locations:
        echo   - external\bgfx\tools\shaderc.exe
        echo   - tools\shaderc\shaderc.exe
        echo.
        echo Please ensure the bgfx shader compiler is available.
        echo.
        pause
        exit /b 1
    )
)

set VARYING=assets\shaders\varying.def.sc
set INCLUDE=external\bgfx\src
set OUTDIR=assets\shaders\compiled

REM Create output directory
if not exist "%OUTDIR%" mkdir "%OUTDIR%"

echo Shader Compiler: %SHADERC%
echo.

echo Compiling vertex shader...
"%SHADERC%" -f assets/shaders/vs_simple.sc -o %OUTDIR%/vs_simple.bin -i %INCLUDE% --varyingdef %VARYING% --platform windows --type vertex --profile s_5_0

echo.
echo Compiling fragment shader...
"%SHADERC%" -f assets/shaders/fs_simple.sc -o %OUTDIR%/fs_simple.bin -i %INCLUDE% --varyingdef %VARYING% --platform windows --type fragment --profile s_5_0

echo.
echo ========================================
echo Shader compilation complete!
echo ========================================
echo.
echo Now rebuild the project in Visual Studio and run it.
echo.
pause
