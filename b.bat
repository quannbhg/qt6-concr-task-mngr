@echo off
rem FULL REBUILD: delete every build folder, then build Debug and Release from scratch.
rem Does not run the app. Close the app first, or the folders cannot be deleted.
setlocal
set "QT_DIR=C:\Qt\6.11.2\mingw_64"
set "PATH=C:\Qt\Tools\mingw1310_64\bin;%QT_DIR%\bin;C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja\bin;%PATH%"
cd /d "%~dp0"

echo === Deleting old build folders
for %%D in (build build-debug build-release) do (
    if exist "%%D" (
        rmdir /s /q "%%D"
        if exist "%%D" (
            echo ERROR: cannot delete %%D - is the app still running?
            exit /b 1
        )
    )
)

echo === Building Debug
cmake -S . -B build-debug -G Ninja -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Debug || exit /b 1
cmake --build build-debug || exit /b 1

echo === Building Release
cmake -S . -B build-release -G Ninja -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release || exit /b 1
cmake --build build-release || exit /b 1

echo === Full build done