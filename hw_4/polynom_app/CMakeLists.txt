cmake_minimum_required(VERSION 3.14)

project(polynom_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(polynom_app
        main.cpp
        Polynomial.cpp
        PolynomialIO.cpp
)