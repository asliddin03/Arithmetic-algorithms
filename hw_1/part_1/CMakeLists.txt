cmake_minimum_required(VERSION 3.20)
project(part_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task_1 task_1/main.cpp)
add_executable(task_2 task_2/main.cpp)
add_executable(task_3 task_3/main.cpp)

add_executable(task_4
        task_4/main.cpp
        task_4/polynomial_utils.cpp
        task_4/limitAtA.cpp
        task_4/limitInf.cpp
)

# чтобы заголовки .hpp искались в папке task_4/
target_include_directories(task_4 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/task_4)
