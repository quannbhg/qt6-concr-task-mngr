find_package(Qt6 REQUIRED COMPONENTS Test Qml)

# Test only need C++, no need QML/view
qt_add_executable(test_taskmodel
    test_TaskModel.cpp
    ../src/TaskModel.cpp
    ../src/TaskRunnable.cpp
    ../src/LogModel.cpp
)

target_include_directories(test_taskmodel PRIVATE ../src)

target_link_libraries(test_taskmodel
    PRIVATE
        Qt6::Test
        Qt6::Qml
)

add_test(NAME test_taskmodel COMMAND test_taskmodel)