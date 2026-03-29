cmake_minimum_required(VERSION 3.14)

project(polynom_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(nlohmann_json CONFIG REQUIRED)

add_executable(polynom_app
        main.cpp
        Polynomial.cpp
        PolynomialIO.cpp
)

target_link_libraries(polynom_app PRIVATE nlohmann_json::nlohmann_json)