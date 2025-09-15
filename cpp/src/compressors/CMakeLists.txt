add_library(compressors STATIC
    Compressor.hpp
    TruncCompressor.cpp
    TruncCompressor.hpp
    SZ3Compressor.cpp
    SZ3Compressor.hpp
)

target_link_libraries(compressors utils ZLIB::ZLIB SZ3::SZ3)