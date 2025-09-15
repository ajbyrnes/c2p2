
/**
 * @file Compressor.hpp
 * @brief Abstract base class for data compressors.
 */
#pragma once

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <cstdint>

struct CompressedData {
    std::vector<uint8_t> data;      // Compressed data
    size_t numFloats;               // Number of floats in original uncompressed data
    std::map<std::string, std::string> compressorConfig;        // Compressor-specific configuration parameters
};

class Compressor {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Compressor() = default;

    virtual std::map<std::string, std::string> getConfig() const = 0;
    virtual std::string toString() const = 0;

    /**
     * @brief Compress the input data.
     * @param data Vector of floats representing the uncompressed data.
     * @return CompressedData structure containing compressed byte data and metadata.
     */
    virtual CompressedData compress(const std::vector<float>& data) = 0;

    /**
     * @brief Decompress the input compressed data.
     * @param compressed CompressedData structure containing compressed byte data and metadata.
     * @return Vector of floats representing the decompressed data.
     */
    virtual std::vector<float> decompress(const CompressedData& compressed) = 0;
};