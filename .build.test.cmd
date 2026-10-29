@echo off
rem Build the Debug version, run the tests with ctest (short summary),
rem then run the test program directly to see every test case.
rem (t.bat is the quick version: it only builds and runs ctest.)
setlocal
set "QT_DIR=C:\Qt\6.11.2\mingw_64"
set "PATH=C:\Qt\Tools\mingw1310_64\bin;%QT_DIR%\bin;C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja\bin;%PATH%"
cd /d "%~dp0"

cmake -S . -B build-debug -G Ninja -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Debug || exit /b 1
cmake --build build-debug || exit /b 1

ctest --test-dir build-debug --output-on-failure || exit /b 1

build-debug\test_taskmodel.exe