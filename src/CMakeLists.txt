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
    -fcommon
)

# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add ZLIB
find_package(ZLIB REQUIRED)

# Add ROOT IO
find_package(ROOT 6.30 CONFIG REQUIRED)

# Add SZ3
set(SZ3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/SZ3/SZ3_install/lib/cmake/SZ3")
find_package(SZ3 REQUIRED)

# Static library "datagen"
add_library(datagen STATIC utils/datagen.cpp utils/datagen.hpp)

# Static library "compressors"
add_library(compressors STATIC 
    compressors/Compressor.hpp 
    compressors/TruncCompressor.cpp compressors/TruncCompressor.hpp
    compressors/SZ3Compressor.cpp compressors/SZ3Compressor.hpp
)
target_link_libraries(compressors ZLIB::ZLIB SZ3::SZ3)

# Static library "utils"
add_library(utils STATIC utils/utils.cpp utils/utils.hpp compressors/Compressor.hpp)
target_link_libraries(utils compressors ROOT::Core ROOT::RIO ROOT::Tree)

# Static library "benchmark"
add_library(benchmarking STATIC 
    benchmarking/CompressorBenchmark.cpp 
    benchmarking/CompressorBenchmark.hpp
)
target_link_libraries(benchmarking compressors utils)

# Add testing executables
add_executable(test-readVectorFloatBranchFromFiles tests/test-readVectorFloatBranchFromFiles.cpp)
target_link_libraries(test-readVectorFloatBranchFromFiles utils)

add_executable(test-writeFloatVectorToFile tests/test-writeFloatVectorToFile.cpp)
target_link_libraries(test-writeFloatVectorToFile utils datagen)

add_executable(test-datagen tests/test-datagen.cpp)
target_link_libraries(test-datagen datagen)

add_executable(test-TruncCompressor tests/test-TruncCompressor.cpp)
target_link_libraries(test-TruncCompressor compressors utils)

add_executable(test-SZ3Compressor tests/test-SZ3Compressor.cpp)
target_link_libraries(test-SZ3Compressor compressors utils)

# Benchmark executables
add_executable(benchmark-TruncCompressor benchmarking/benchmark-TruncCompressor.cpp)
target_link_libraries(benchmark-TruncCompressor benchmarking)

add_executable(benchmark-SZ3Compressor benchmarking/benchmark-SZ3Compressor.cpp)
target_link_libraries(benchmark-SZ3Compressor benchmarking)

add_executable(benchmark-SZ3SanityCheck benchmarking/benchmark-SZ3SanityCheck.cpp)
target_link_libraries(benchmark-SZ3SanityCheck benchmarking datagen)