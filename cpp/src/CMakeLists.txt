
cmake_minimum_required(VERSION 3.15)
project(c2p2 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
find_package(nlohmann_json CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SZ3 REQUIRED)

add_subdirectory(utils)
add_subdirectory(compressors)
add_subdirectory(benchmarking)
add_subdirectory(tests)