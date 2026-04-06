cmake_minimum_required(VERSION 3.16)
project(hw_6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(hw_6
        main.cpp
        MainWindow.cpp
        math/MonomialIdeal.cpp
        io/FileReader.cpp
        ui/IdealGrid.cpp
)

target_link_libraries(hw_6 PRIVATE Qt6::Widgets)