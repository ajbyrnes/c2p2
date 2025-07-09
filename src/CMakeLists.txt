cmake_minimum_required(VERSION 3.15)
project(c2p2 CXX)

# Set the C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Additional compiler flags
add_compile_options(
    -pedantic
    -Wall
    -Wextra
    -Wno-unused-parameter
)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add ZLIB
find_package(ZLIB REQUIRED)

# Add ROOT IO
find_package(ROOT 6.36 CONFIG REQUIRED)

# Add SZ3
# find_package(SZ3 REQUIRED)

# Static library "utils"
add_library(utils STATIC utils/utils.cpp utils/utils.hpp)
target_link_libraries(utils ROOT::Core ROOT::RIO ROOT::Tree)

# Static library "compressors"
add_library(compressors STATIC 
    compressors/Compressor.hpp 
    compressors/TruncCompressor.cpp compressors/TruncCompressor.hpp
)
target_link_libraries(compressors ZLIB::ZLIB)

# Static library "benchmark"
add_library(benchmarking STATIC 
    benchmarking/CompressorBenchmark.cpp 
    benchmarking/CompressorBenchmark.hpp
)
target_link_libraries(benchmarking compressors utils)

# Add testing executables
add_executable(test-readVectorFloatBranchFromFiles tests/test-readVectorFloatBranchFromFiles.cpp)
target_link_libraries(test-readVectorFloatBranchFromFiles utils)

add_executable(test-TruncCompressor tests/test-TruncCompressor.cpp)
target_link_libraries(test-TruncCompressor compressors utils)

add_executable(benchmark benchmarking/benchmark.cpp)
target_link_libraries(benchmark benchmarking)

add_executable(benchmark-TruncCompressor benchmarking/benchmarkTruncCompressor.cpp)
target_link_libraries(benchmark-TruncCompressor benchmarking)