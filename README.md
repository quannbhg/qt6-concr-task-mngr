# Concurrent Task Monitor

**English** | [Tiếng Việt](README.vi.md)

A small desktop app built with **Qt 6, C++ and QML**. It runs background
tasks in a thread pool and shows their state, progress and log in real time.

<p align="left">
  <img src="https://img.shields.io/badge/Qt-6.5%2B-41CD52?logo=qt&logoColor=white" alt="Qt 6.5+">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white" alt="C++17">
  <img src="https://img.shields.io/badge/CMake-3.21%2B-064F8C?logo=cmake&logoColor=white" alt="CMake 3.21+">
  <img src="https://img.shields.io/badge/platform-Windows-0078D6?logo=windows&logoColor=white" alt="Windows">
  <img src="https://img.shields.io/badge/license-MIT-yellow" alt="MIT license">
</p>

<p align="left">
  <img src="docs/images/main.png" width="374" alt="Main window">
  <img src="docs/images/demo.gif" width="374" alt="Demo">
</p>

## Features

- Add a task by name. It runs on a worker thread (`QThreadPool`, at most
  2 tasks at the same time; the others wait as **Pending**)
- Task states: Pending, Running, Paused, Completed, Failed, Cancelled
- Real-time progress bar for each task
- **Pause / Resume** a running task
- **Cancel** a task (Pending, Running or Paused)
- **Clear finished** tasks from the list
- Real-time **log panel** (time, message, errors in red, max 500 lines, Clear log button)
- Safe shutdown: closing the app cancels all tasks and waits for the
  worker threads to finish
- Input validation: empty task names are rejected
- Type the name `fail` to create a task that fails at 50% (to test errors)

## Requirements

- Qt 6.5 or newer (tested with Qt 6.11.2, MinGW 64-bit)
- CMake 3.21 or newer
- Ninja
- A C++17 compiler (MinGW from the Qt installer)

## Build

Debug build:

```powershell
cmake -S . -B build-debug -G Ninja -DCMAKE_PREFIX_PATH="<Qt path>" -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug
```

Release build (default if no build type is given):

```powershell
cmake -S . -B build-release -G Ninja -DCMAKE_PREFIX_PATH="<Qt path>" -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

`<Qt path>` is the folder of your Qt kit, for example `C:\Qt\6.11.2\mingw_64`.
Make sure the MinGW and Ninja folders of Qt are in your `PATH`.

## Build scripts (Windows)

To avoid typing the commands above, the project root has small batch
scripts. Run them from PowerShell or cmd, inside the project folder
(for example `.\m.bat`).

| Script | What it does |
|---|---|
| `b.bat` | Full rebuild: deletes `build`, `build-debug` and `build-release`, then builds Debug and Release from scratch. Close the app first. |
| `m.bat` | Incremental build: keeps the build folders and rebuilds only what changed. Optional argument `debug` or `release`, otherwise it builds both. |
| `t.bat` | Incremental build, then runs the tests with CTest. Optional argument `debug` or `release`, otherwise it tests both. |
| `.build.debug.cmd` | Builds Debug, then runs the app inside the terminal (console output stays visible). |
| `.build.release.cmd` | Builds Release, then starts the app in its own window. |
| `.build.test.cmd` | Builds Debug, runs CTest, then runs the test program to list every test case. |

The scripts expect Qt at `C:\Qt\6.11.2\mingw_64` and the MinGW, CMake and
Ninja tools of the Qt installer under `C:\Qt\Tools`. If yours are somewhere
else, edit the `QT_DIR` and `PATH` lines at the top of each script.

## Run

```powershell
.\build-release\ConcurrentTaskMonitor.exe
```

If the window does not appear, Windows probably cannot find the Qt DLLs.
Either add the Qt `bin` folders to `PATH`, or deploy them next to the exe:

```powershell
windeployqt6 --release --qmldir . build-release\ConcurrentTaskMonitor.exe
```

## Test

The tests use Qt Test and run without a UI. The easiest way is the script,
which builds what is needed and runs the tests for Debug and Release:

```powershell
.\t.bat
```

Or run CTest yourself:

```powershell
ctest --test-dir build-release --output-on-failure
```

See [docs/testing.md](docs/testing.md) for what is tested and the results.

## Project structure

```
.
├── CMakeLists.txt
├── b.bat, m.bat, t.bat    full rebuild, incremental build, build and test
├── .build.*.cmd           build and run scripts (debug, release, test)
├── main.cpp               app entry point
├── Main.qml               user interface
├── src/
│   ├── Task.h             task data and status enum
│   ├── TaskRunnable.*     worker that runs in the thread pool
│   ├── TaskModel.*        task list model shared with QML
│   └── LogModel.*         log list model shared with QML
├── test/                  Qt Test suite for TaskModel
└── docs/                  architecture, testing, images
```

## How it works

Worker threads never touch the UI. They only emit signals; Qt delivers
them to `TaskModel` on the main thread (queued connection), and the model
updates the QML list. Cancel and pause use shared atomic flags that the
worker checks in its loop.

See [docs/architecture.md](docs/architecture.md) for diagrams.

## Known limitations

- Tasks are simulated (100 steps of 50 ms), they do not do real work yet
- Tested on Windows with MinGW only

## Roadmap

- Configurable task duration
- Task priority
- Real work types instead of the simulated one

## License

MIT, see [LICENSE](LICENSE).