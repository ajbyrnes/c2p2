#pragma once

#include <string>
#include <vector>

// Abstract base class for a compressor
class Compressor {
public:
    virtual ~Compressor() = default;

    // Compress input data, returns compressed data
    virtual std::vector<uint8_t> compress(const std::vector<float>& data) = 0;

    // Decompress input data, returns decompressed data
    virtual std::vector<float> decompress(const std::vector<uint8_t>& compressedData) = 0;

    // Name of the compressor
    virtual std::string name() const = 0;
};
