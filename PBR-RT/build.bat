@echo off
REM Anito Engine Build System
REM Synchronized with CMakeLists.txt configuration
REM Generator: Visual Studio 18 2026
REM Platform: x64
REM CMake Min: 3.10
REM C++ Standard: C++20

setlocal

REM Parse command and configuration
set COMMAND=%1
set BUILD_CONFIG=%2
if "%COMMAND%"=="" set COMMAND=build
if "%BUILD_CONFIG%"=="" set BUILD_CONFIG=Release

REM Supported configurations
if /i "%BUILD_CONFIG%"=="Debug" goto valid_config
if /i "%BUILD_CONFIG%"=="Release" goto valid_config
if /i "%BUILD_CONFIG%"=="RelWithDebInfo" goto valid_config
if /i "%BUILD_CONFIG%"=="MinSizeRel" goto valid_config
if not "%BUILD_CONFIG:~0,2%"=="--" (
    echo Invalid configuration: %BUILD_CONFIG%
    echo Valid options: Debug, Release, RelWithDebInfo, MinSizeRel
    exit /b 1
)
:valid_config

REM Route commands
if /i "%COMMAND%"=="setup" goto setup
if /i "%COMMAND%"=="build" goto build
if /i "%COMMAND%"=="rebuild" goto rebuild
if /i "%COMMAND%"=="run" goto run
if /i "%COMMAND%"=="clean" goto clean
if /i "%COMMAND%"=="check" goto check
if /i "%COMMAND%"=="validate" goto validate
if /i "%COMMAND%"=="shaderc" goto shaderc
if /i "%COMMAND%"=="help" goto help
echo Unknown command: %COMMAND%
echo Run "Build.bat help" for usage
exit /b 1

:setup
echo ==================================================
echo  Anito Engine Setup
echo ==================================================
call :check || exit /b 1
echo.
echo Checking CMake version...
cmake --version | findstr /R "cmake version [3-9]\." >nul || (
    echo ERROR: CMake 3.10 or newer required
    echo Please download from: https://cmake.org/download/
    exit /b 1
)
for /f "tokens=3" %%v in ('cmake --version ^| findstr /R "cmake version"') do set CMAKE_VERSION=%%v
echo CMake version: %CMAKE_VERSION%
echo.
echo Initializing submodules...
git submodule update --init --recursive || (
    echo WARNING: Failed to update submodules
    echo This may cause build failures if dependencies are missing
)
echo.
echo Configuring CMake with Visual Studio 18 2026 (x64)...
cmake -B build -G "Visual Studio 18 2026" -A x64 ^
    -DCMAKE_CXX_STANDARD=20 ^
    -DANITO_BUILD_SAMPLES=ON ^
    -DANITO_USE_VULKAN=ON ^
    -DANITO_COMPILE_SHADERS=ON || exit /b 1
echo.
echo ==================================================
echo  Setup Complete
echo ==================================================
echo Build directory: build/
echo Generator: Visual Studio 18 2026
echo Platform: x64
echo C++ Standard: C++20
echo ==================================================
exit /b 0

:build
if not exist build (
    echo Build directory not found. Running setup...
    call :setup || exit /b 1
)
echo ==================================================
echo  Building Anito Engine [%BUILD_CONFIG%]
echo ==================================================
cmake --build build --config %BUILD_CONFIG% --parallel || exit /b 1
echo.
echo ==================================================
echo  Build Successful [%BUILD_CONFIG%]
echo ==================================================
echo Executable: build\bin\%BUILD_CONFIG%\AnitoEngine.exe
echo ==================================================
exit /b 0

:rebuild
echo ==================================================
echo  Rebuilding Anito Engine [%BUILD_CONFIG%]
echo ==================================================
call :clean
call :setup || exit /b 1
call :build %BUILD_CONFIG%
exit /b 0

:run
call :build %BUILD_CONFIG% || exit /b 1
echo.
echo ==================================================
echo  Running Anito Engine [%BUILD_CONFIG%]
echo ==================================================
if exist build\bin\%BUILD_CONFIG%\AnitoEngine.exe (
    build\bin\%BUILD_CONFIG%\AnitoEngine.exe
) else (
    echo ERROR: Executable not found at build\bin\%BUILD_CONFIG%\AnitoEngine.exe
    echo Run "Build.bat build %BUILD_CONFIG%" first
    exit /b 1
)
exit /b 0

:clean
echo ==================================================
echo  Cleaning Build Directory
echo ==================================================
if exist build (
    rmdir /s /q build
    echo Build directory removed
) else (
    echo Build directory does not exist
)
exit /b 0

:check
echo ==================================================
echo  Checking Build Requirements
echo ==================================================
echo Checking for Git...
where git >nul 2>nul || (
    echo ERROR: Git not found in PATH
    echo Download from: https://git-scm.com/download/win
    exit /b 1
)
for /f "tokens=3" %%v in ('git --version') do echo   Git version: %%v

echo Checking for CMake...
where cmake >nul 2>nul || (
    echo ERROR: CMake not found in PATH
    echo Download from: https://cmake.org/download/
    exit /b 1
)
for /f "tokens=3" %%v in ('cmake --version ^| findstr /R "cmake version"') do echo   CMake version: %%v

echo Checking for Visual Studio 2026...
if exist "C:\Program Files\Microsoft Visual Studio\2026\Community\Common7\IDE\devenv.exe" (
    echo   Visual Studio 2026 Community: Found
) else if exist "C:\Program Files\Microsoft Visual Studio\2026\Professional\Common7\IDE\devenv.exe" (
    echo   Visual Studio 2026 Professional: Found
) else if exist "C:\Program Files\Microsoft Visual Studio\2026\Enterprise\Common7\IDE\devenv.exe" (
    echo   Visual Studio 2026 Enterprise: Found
) else (
    echo WARNING: Visual Studio 2026 not found
    echo CMake will attempt to use available compiler
)

echo.
echo ==================================================
echo  All required tools found
echo ==================================================
exit /b 0

:validate
echo ==================================================
echo  Validating Build Configuration
echo ==================================================
if not exist tools\validate_build_config.ps1 (
    echo ERROR: tools\validate_build_config.ps1 not found
    exit /b 1
)
powershell -ExecutionPolicy Bypass -File tools\validate_build_config.ps1
exit /b %ERRORLEVEL%

:shaderc
echo ==================================================
echo  Building Shader Compiler (shaderc)
echo ==================================================
if not exist tools\build_shaderc.bat (
    echo ERROR: tools\build_shaderc.bat not found
    exit /b 1
)
call tools\build_shaderc.bat
exit /b %ERRORLEVEL%

:help
echo ==================================================
echo  Anito Engine Build System - Usage
echo ==================================================
echo.
echo COMMANDS:
echo   Build.bat setup                  - Initialize build system
echo   Build.bat build [config]         - Build project (default: Release)
echo   Build.bat rebuild [config]       - Clean and rebuild
echo   Build.bat run [config]           - Build and run (default: Release)
echo   Build.bat clean                  - Remove build directory
echo   Build.bat check                  - Verify build requirements
echo   Build.bat validate               - Validate Build.bat and CMakeLists.txt sync
echo   Build.bat shaderc                - Build shader compiler
echo   Build.bat help                   - Show this help
echo.
echo CONFIGURATIONS:
echo   Debug                            - Debug build with symbols
echo   Release                          - Optimized release build (default)
echo   RelWithDebInfo                   - Release with debug info
echo   MinSizeRel                       - Minimum size release
echo.
echo EXAMPLES:
echo   Build.bat                        - Build in Release mode
echo   Build.bat build Debug            - Build in Debug mode
echo   Build.bat run Debug              - Build and run Debug version
echo   Build.bat rebuild Release        - Clean and rebuild Release
echo   Build.bat validate               - Check configuration sync
echo.
echo SYNCHRONIZED WITH:
echo   CMakeLists.txt
echo   Generator: Visual Studio 18 2026
echo   Platform: x64
echo   C++ Standard: C++20
echo   CMake Min Version: 3.10
echo.
echo ==================================================
exit /b 0
