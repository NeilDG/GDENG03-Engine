@echo off
REM ============================================================================
REM Build shaderc tool from bgfx using GENie
REM ============================================================================

echo.
echo ========================================
echo Building shaderc for Anito Engine
echo ========================================
echo.

if not exist "external\bgfx" (
    echo [ERROR] bgfx directory not found at external\bgfx
    echo Please ensure Git submodules are initialized
    pause
    exit /b 1
)

if not exist "external\bx" (
    if exist "external\bgfx\bx" (
        echo [INFO] Using bx from bgfx\bx
    ) else (
        echo [ERROR] bx directory not found
        pause
        exit /b 1
    )
)

REM Detect Visual Studio version
where cl.exe >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo [INFO] Visual Studio command line tools not in PATH
    echo Attempting to locate Visual Studio...

    REM Try to find vswhere
    set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

    if exist "%VSWHERE%" (
        for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
            set "VS_PATH=%%i"
        )
    )

    if defined VS_PATH (
        echo [INFO] Found Visual Studio at: %VS_PATH%

        if exist "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" (
            echo [INFO] Setting up Visual Studio environment...
            call "%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat" x64
        )
    )
)

REM Detect platform architecture
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set PLATFORM_ARCH=win64
) else if "%PROCESSOR_ARCHITECTURE%"=="x86" (
    set PLATFORM_ARCH=win32
) else (
    set PLATFORM_ARCH=win64
)

echo [INFO] Target platform: %PLATFORM_ARCH%
echo.

REM Detect Visual Studio version for GENie
set GENIE_VS=vs2022
if defined VisualStudioVersion (
    if "%VisualStudioVersion:~0,2%"=="17" set GENIE_VS=vs2022
    if "%VisualStudioVersion:~0,2%"=="16" set GENIE_VS=vs2019
    if "%VisualStudioVersion:~0,2%"=="15" set GENIE_VS=vs2017
)
echo [INFO] Using Visual Studio configuration: %GENIE_VS%
echo.

REM Find GENie executable
set GENIE_EXE=
if exist "external\bx\tools\bin\windows\genie.exe" (
    set GENIE_EXE=external\bx\tools\bin\windows\genie.exe
) else if exist "external\bgfx\bx\tools\bin\windows\genie.exe" (
    set GENIE_EXE=external\bgfx\bx\tools\bin\windows\genie.exe
) else if exist "tools\genie.exe" (
    set GENIE_EXE=tools\genie.exe
)

if not defined GENIE_EXE (
    echo [ERROR] GENie not found!
    echo.
    echo Please download GENie from: https://github.com/bkaradzic/GENie/releases
    echo And place it in one of these locations:
    echo   - external\bx\tools\bin\windows\genie.exe
    echo   - external\bgfx\bx\tools\bin\windows\genie.exe
    echo   - tools\genie.exe
    echo.
    pause
    exit /b 1
)

echo [OK] Found GENie at: %GENIE_EXE%
echo.

REM Navigate to bgfx directory
pushd external\bgfx

echo Generating project files with GENie...
..\..\%GENIE_EXE% --with-tools %GENIE_VS%

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] GENie failed to generate project files
    popd
    pause
    exit /b 1
)

echo.
echo [OK] Project files generated
echo.

REM Try to build using MSBuild
echo Attempting to build shaderc...
echo.

set SOLUTION_PATH=.build\projects\%GENIE_VS%\bgfx.sln

if not exist "%SOLUTION_PATH%" (
    echo [WARNING] Solution file not found at: %SOLUTION_PATH%
    echo.
    echo Please manually open the solution and build the shaderc project:
    echo   %cd%\%SOLUTION_PATH%
    echo.
    echo Then copy the built shaderc.exe to one of these locations:
    echo   - tools\shaderc.exe
    echo   - external\bgfx\tools\shaderc.exe
    popd
    pause
    exit /b 0
)

REM Find MSBuild
set MSBUILD_EXE=
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe 2^>nul`) do (
    set "MSBUILD_EXE=%%i"
    goto :found_msbuild
)
:found_msbuild

if not defined MSBUILD_EXE (
    echo [WARNING] MSBuild not found
    echo.
    echo Please manually open the solution and build the shaderc project:
    echo   %cd%\%SOLUTION_PATH%
    echo.
    popd
    pause
    exit /b 0
)

echo [OK] Found MSBuild at: %MSBUILD_EXE%
echo.

echo Building shaderc (Release configuration)...
"%MSBUILD_EXE%" "%SOLUTION_PATH%" /p:Configuration=Release /p:Platform=x64 /t:shaderc /m

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [WARNING] MSBuild encountered errors
    echo.
    echo Please manually open the solution and build the shaderc project:
    echo   %cd%\%SOLUTION_PATH%
    echo.
    popd
    pause
    exit /b 0
)

popd

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.

REM Look for the built shaderc.exe
echo Searching for built shaderc.exe...

set FOUND_SHADERC=
for /r "external\bgfx\.build" %%F in (shaderc.exe shadercRelease.exe shadercDebug.exe) do (
    if exist "%%F" (
        set FOUND_SHADERC=%%F
        echo [OK] Found: %%F
    )
)

if defined FOUND_SHADERC (
    echo.
    echo Creating tools directory...
    if not exist "tools" mkdir tools

    echo Copying shaderc.exe to tools directory...
    copy /Y "%FOUND_SHADERC%" "tools\shaderc.exe"

    echo.
    echo [SUCCESS] shaderc.exe is now available at: tools\shaderc.exe
    echo.
    echo Please reconfigure CMake to enable shader compilation:
    echo   cmake -B build
    echo.
) else (
    echo.
    echo [WARNING] Could not locate built shaderc.exe
    echo Please check: external\bgfx\.build
    echo And manually copy shaderc.exe to: tools\shaderc.exe
    echo.
)

echo ========================================
pause
