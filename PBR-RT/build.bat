@echo off
REM Anito Engine Build System
setlocal
set COMMAND=%1
if "%COMMAND%"=="" set COMMAND=build
if /i "%COMMAND%"=="setup" goto setup
if /i "%COMMAND%"=="build" goto build
if /i "%COMMAND%"=="run" goto run
if /i "%COMMAND%"=="clean" goto clean
if /i "%COMMAND%"=="check" goto check
echo Unknown command: %COMMAND%
exit /b 1
:setup
echo Setting up...
git submodule update --init --recursive || exit /b 1
cmake -B build || exit /b 1
echo Setup complete
exit /b 0
:build
if not exist build call :setup
echo Building...
cmake --build build --config Release || exit /b 1
echo Build successful
exit /b 0
:run
call :build || exit /b 1
if exist build\bin\Release\AnitoEngine.exe (
    build\bin\Release\AnitoEngine.exe
) else (
    echo Executable not found
    exit /b 1
)
exit /b 0
:clean
if exist build rmdir /s /q build
exit /b 0
:check
where git >nul 2>nul || (echo Git missing & exit /b 1)
where cmake >nul 2>nul || (echo CMake missing & exit /b 1)
echo All tools found
exit /b 0
