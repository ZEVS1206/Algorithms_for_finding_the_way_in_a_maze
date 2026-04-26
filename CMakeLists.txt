cmake_minimum_required(VERSION 3.15)
project(IMPLEMENTATION_OF_JPS4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

include_directories(include)
file(GLOB SOURCES "source/*.cpp")
add_executable(start ${SOURCES})

target_link_libraries(start PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(start PRIVATE ${nlohmann_json_SOURCE_DIR}/include)
