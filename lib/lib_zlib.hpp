#include <algorithm>
#include <cstdint>
#include <vector>

#include <zlib.h>

#include "lib_utils.hpp"

// Bit truncation -------------------------------------------------------------------------------------------------

float truncateFloat(float value, const int& bits, const bool& round) {
    // Do nothing for 0 bits
    if (bits == 0) return value;

    // Only mantissa bits should be truncated
    if (bits < 0 || bits > 23) {
        throw std::runtime_error("truncateNoRounding: bits must be between 0 and 23");
    }

    // Convert float to int
    uint32_t intVal{*reinterpret_cast<uint32_t*>(&value)};

    // Create masks for dropping and keeping bits
    uint32_t dropMask{((1u << bits) - 1u)};
    uint32_t keepMask{~dropMask};        // This is also the maximum truncated value

    // Truncate value
    uint32_t truncatedIntVal{intVal & keepMask};

    // Round-to-even if value won't overflow
    if (round && truncatedIntVal < keepMask) {
        // Round-to-even has us round up if the truncated bits are greater than 2^(bits - 1)
        // Ex: If bits = 4, we round up if the 4 dropped bits are greater than 2^3 = 1000
        uint32_t droppedVal{intVal & dropMask};      // The dropped bits
        uint32_t roundUpLimit{(1u << (bits - 1u))};

        if (droppedVal > roundUpLimit) {
            truncatedIntVal = ((truncatedIntVal >> bits) + 1u) << bits;
        }
    }
    
    // Return truncated value as float
    return *reinterpret_cast<float*>(&truncatedIntVal);
}

std::vector<float> truncateVectorData(const std::vector<float>& data, const int& bits) {
    // Allocate truncated basket
    std::vector<float> truncatedData(data.size());

    // Truncate basket
    std::transform(data.begin(), data.end(), truncatedData.begin(), 
                [bits](float value) { return truncateFloat(value, bits, true); });

    return truncatedData;
}

// zlib compression ----------------------------------------------------------------------------------------------
// Modified from https://gitlab.cern.ch/-/snippets/3301

CompressorOut zlibCompress(const CompressorIn& data, const CompressorParams& params) {
    // Allocate compressed basket
    size_t compressedSize{compressBound(data.size() * sizeof(float))};
    std::vector<uint8_t> compressedData(compressedSize);

    // Compress
    int result = compress2(compressedData.data(), &compressedSize, reinterpret_cast<const uint8_t*>(data.data()), 
                          data.size() * sizeof(float));

    if (result != Z_OK) {
        throw std::runtime_error("zlibCompress: compression failed");
    }

    // Resize to actual size
    compressedData.resize(compressedSize);

    // Return compressed data
    return compressedData;
}

CompressorOut zlibTruncateCompress(const CompressorIn& data, const CompressorParams& params) {
    // Truncate data
    std::vector<float> truncatedData = truncateVectorData(data, params.at("bits"));

    // Compress truncated data
    return zlibCompress(truncatedData, params);
}

DecompressorOut zlibDecompress(const DecompressorIn& compressedData, const size_t& uncompressedSize) {
    // Allocate decompressed basket
    std::vector<float> decompressedData(uncompressedSize / sizeof(float));

    // Decompress
    size_t uncompressedSizeBytes = uncompressedSize * sizeof(float);
    int result = uncompress(reinterpret_cast<uint8_t*>(decompressedData.data()), &uncompressedSizeBytes,
                            compressedData.data(), compressedData.size());

    if (result != Z_OK) {
        throw std::runtime_error("zlibDecompress: decompression failed");
    }

    return decompressedData;
}