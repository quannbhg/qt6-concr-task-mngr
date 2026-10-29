@echo off
rem Build the Release version, then start it.
rem "start" opens the app in its own window, so this terminal is free again.
setlocal
set "QT_DIR=C:\Qt\6.11.2\mingw_64"
set "PATH=C:\Qt\Tools\mingw1310_64\bin;%QT_DIR%\bin;C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja\bin;%PATH%"
cd /d "%~dp0"

cmake -S . -B build-release -G Ninja -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release || exit /b 1
cmake --build build-release || exit /b 1

start "" "build-release\ConcurrentTaskMonitor.exe"