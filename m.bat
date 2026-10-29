@echo off
rem INCREMENTAL BUILD: keep the build folders, only rebuild what changed.
rem Ninja re-runs CMake by itself when a CMakeLists.txt changed.
rem A build folder is configured only if it does not exist yet.
rem
rem Usage:  m.bat            build Debug and Release
rem         m.bat debug      build Debug only
rem         m.bat release    build Release only
setlocal
set "QT_DIR=C:\Qt\6.11.2\mingw_64"
set "PATH=C:\Qt\Tools\mingw1310_64\bin;%QT_DIR%\bin;C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja\bin;%PATH%"
cd /d "%~dp0"

set "MODE=%~1"
if "%MODE%"=="" set "MODE=all"

if /i "%MODE%"=="debug"   goto :debug
if /i "%MODE%"=="release" goto :release
if /i "%MODE%"=="all"     goto :debug
echo Usage: m.bat [debug^|release]
exit /b 1

:debug
call :build Debug build-debug || exit /b 1
if /i "%MODE%"=="debug" goto :done

:release
call :build Release build-release || exit /b 1
goto :done

:build
rem %1 = build type, %2 = build folder
echo === Building %~1
if not exist "%~2\build.ninja" (
    cmake -S . -B "%~2" -G Ninja -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=%~1 || exit /b 1
)
cmake --build "%~2" || exit /b 1
exit /b 0

:done
echo === Build done