cmake_minimum_required(VERSION 3.16)

project(hw_11 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

qt_standard_project_setup()

qt_add_executable(hw_11
        main.cpp
        mainwindow.cpp
        powersum.cpp
        main.cpp
        powersum.cpp
        powersum.h
        mainwindow.cpp
        mainwindow.h
)

target_link_libraries(hw_11
        PRIVATE
        Qt6::Widgets
        Qt6::Charts
)