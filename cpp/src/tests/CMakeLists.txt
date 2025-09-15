# tests/CMakeLists.txt

add_executable(test-TruncCompressor test-TruncCompressor.cpp)
target_link_libraries(test-TruncCompressor compressors utils)

add_executable(test-SZ3Compressor test-SZ3Compressor.cpp)
target_link_libraries(test-SZ3Compressor compressors utils)

add_executable(test-TTreeRead test-TTreeRead.cpp)
target_link_libraries(test-TTreeRead utils)

add_executable(ttree-experiment ttree-experiment.cpp)
target_link_libraries(ttree-experiment utils)

add_executable(test-JSON test-JSON.cpp)
target_link_libraries(test-JSON compressors utils)