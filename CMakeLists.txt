cmake_minimum_required(VERSION 3.15)
project(IMPLEMENTATION_OF_JPS4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)
file(GLOB SOURCES "source/*.cpp")

add_executable(start ${SOURCES})
