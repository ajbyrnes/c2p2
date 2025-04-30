#ifndef MY_COMPRESSOR_HPP
#define MY_COMPRESSOR_HPP

#include <cstdint>
#include <vector>

class MyCompressor{
    public:
        virtual ~MyCompressor() = default;

        // Compress vector of floats into vector of bytes
        virtual std::vector<uint8_t> compress(const std::vector<float>& data) = 0;

        // Decompress vector of bytes into vector of floats
        virtual std::vector<float> decompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) = 0;
};

#endif