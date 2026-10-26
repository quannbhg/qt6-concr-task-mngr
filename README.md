# Concurrent Task Monitor
A small desktop app built with Qt 6, C++ and QML. It runs background tasks in a thread pool and shows their state and progress live.

## Features (current)
- Add a task by name; it runs on a worker thread (QThreadPool, max 2
  tasks at the same time, the others wait as Pending)
- Task states: Pending, Running, Completed, Failed, Cancelled
- Real-time progress bar for each task
- Cancel a task (Pending or Running)
- Clear all finished tasks from the list
- Safe shutdown: closing the app cancels running tasks and waits forcthe threads to finish
- Type the name "fail" to test the Failed state

## Structure
- src/Task.h            task data and status enum
- src/TaskRunnable.*    worker that runs in the thread pool
- src/TaskModel.*       list model shared with QML
- Main.qml              user interface
- main.cpp              app entry point

## How it works
Worker threads never touch the UI. They only emit signals; Qt delivers
them to TaskModel on the main thread (queued connection), and the
model updates the QML list.

## Build
Requires Qt 6.5+ (tested with 6.11.2, MinGW 64-bit), CMake 3.21+
and Ninja.
``bash
    cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="<Qt path>"
``
``bash
    cmake --build build
``
On Windows, run windeployqt6 on the exe (or add Qt to PATH) before
running it.

## Roadmap
- Pause / Resume
- Real-time log and error panel
- Unit tests (Qt Test)
- Debug / Release configs, docs and demo