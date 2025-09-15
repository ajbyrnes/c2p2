# benchmarking/CMakeLists.txt

add_library(benchmarking STATIC
    CompressorBenchmark.cpp
    CompressorBenchmark.hpp
)
target_link_libraries(benchmarking compressors utils)

# add_executable(benchmark-TruncCompressor benchmark-TruncCompressor.cpp)
# target_link_libraries(benchmark-TruncCompressor benchmarking)

# add_executable(benchmark-SZ3Compressor benchmark-SZ3Compressor.cpp)
# target_link_libraries(benchmark-SZ3Compressor benchmarking)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark benchmarking)