cmake_minimum_required(VERSION 3.21)

project(ConcurrentTaskMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(ConcurrentTaskMonitor
    main.cpp
)

# Bundle C++ code (classes used in QML) and QML files into a single module
qt_add_qml_module(ConcurrentTaskMonitor
    URI TaskMonitor
    VERSION 1.0
    QML_FILES
        Main.qml
    SOURCES
        src/Task.h
        src/TaskRunnable.h
        src/TaskRunnable.cpp
        src/TaskModel.h
        src/TaskModel.cpp
        src/LogModel.h
        src/LogModel.cpp
)

# Tell the compiler to look for .h files in the src directory
target_include_directories(ConcurrentTaskMonitor PRIVATE src)

enable_testing()
add_subdirectory(test)

target_link_libraries(ConcurrentTaskMonitor
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
)