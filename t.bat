@echo off
rem BUILD AND RUN THE TESTS (incremental build first, then ctest).
rem
rem Usage:  t.bat            test Debug and Release
rem         t.bat debug      test Debug only
rem         t.bat release    test Release only
setlocal
set "QT_DIR=C:\Qt\6.11.2\mingw_64"
set "PATH=C:\Qt\Tools\mingw1310_64\bin;%QT_DIR%\bin;C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja\bin;%PATH%"
cd /d "%~dp0"

set "MODE=%~1"
if "%MODE%"=="" set "MODE=all"

call "%~dp0m.bat" %MODE% || exit /b 1

if /i not "%MODE%"=="release" (
    echo === Testing Debug
    ctest --test-dir build-debug --output-on-failure || exit /b 1
)

if /i not "%MODE%"=="debug" (
    echo === Testing Release
    ctest --test-dir build-release --output-on-failure || exit /b 1
)

echo === All tests passed