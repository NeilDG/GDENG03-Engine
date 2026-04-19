@echo off
REM Build shaderc from bgfx source using GENie

echo ========================================
echo Building shaderc from bgfx source
echo ========================================
echo.

cd external\bgfx

REM Check if GENie exists
if not exist "bx\tools\bin\windows\genie.exe" (
    echo ERROR: GENie not found at bgfx\bx\tools\bin\windows\genie.exe
    echo.
    echo GENie should be included with bgfx/bx. Please verify your bgfx setup.
    pause
    exit /b 1
)

echo Running GENie to generate Visual Studio projects...
bx\tools\bin\windows\genie.exe --with-tools vs2022

if errorlevel 1 (
    echo ERROR: GENie failed to generate projects
    pause
    exit /b 1
)

echo.
echo Projects generated in: .build\projects\vs2022
echo.
echo Now building shaderc in Release mode...
echo.

REM Build using MSBuild
set MSBUILD="C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"

if not exist %MSBUILD% (
    echo ERROR: MSBuild not found at: %MSBUILD%
    echo Please adjust the path to match your Visual Studio installation
    pause
    exit /b 1
)

%MSBUILD% .build\projects\vs2022\shaderc.vcxproj /p:Configuration=Release /p:Platform=x64

if errorlevel 1 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

cd ..\..

echo.
echo ========================================
echo Build complete!
echo ========================================
echo.

REM Find the built shaderc.exe
set SHADERC_BUILT=external\bgfx\.build\win64_vs2022\bin\shadercRelease.exe

if exist %SHADERC_BUILT% (
    echo shaderc built at: %SHADERC_BUILT%
    echo.
    echo Copying to tools\shaderc\shaderc.exe...
    if not exist "tools\shaderc" mkdir "tools\shaderc"
    copy /Y %SHADERC_BUILT% tools\shaderc\shaderc.exe
    echo.
    echo SUCCESS! shaderc.exe is ready.
    echo.
    echo Now reconfigure CMake and rebuild your project.
) else (
    echo WARNING: Could not find built shaderc at expected location.
    echo Please check: external\bgfx\.build\
    dir /s /b external\bgfx\.build\shaderc*.exe
)

echo.
pause
