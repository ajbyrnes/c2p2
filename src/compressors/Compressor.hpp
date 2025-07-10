#pragma once

#include <map>
#include <string>
#include <vector>

#include <cstdint>

struct UncompressedData {
    std::vector<float> data; // Uncompressed data
    std::string dataName;
    std::string fileName{}; // Name of the file containing the data
};

struct CompressedData {
    std::vector<uint8_t> data; // Compressed data
    size_t numFloats;          // Number of floats in the original data
};

// Abstract base class for a compressor
class Compressor {
public:
    virtual ~Compressor() = default;

    // Compress input data, returns compressed data
    virtual CompressedData compress(const UncompressedData& data) = 0;

    // Decompress input data, returns decompressed data
    virtual std::vector<float> decompress(const CompressedData& compressedData) = 0;

    // Reporting
    virtual std::map<std::string, std::string> getConfig() const = 0;
    virtual std::string toString() const = 0;
};
