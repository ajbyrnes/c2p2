#include <algorithm>
#include <cstdint>
#include <vector>
#include <zlib.h>

#include "lib_utils.hpp"

#ifndef LIB_ZLIB_HPP
#define LIB_ZLIB_HPP

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

template<typename T>
std::vector<uint8_t> zlibCompress(const std::vector<T>& data, const int& level=Z_DEFAULT_COMPRESSION) {
    // Calculate length of uncompressed data
    uLong sourceLen{data.size() * sizeof(T)};

    // Calculate maximum length of compressed data
    uLong destLen{compressBound(sourceLen)};

    // Allocate memory for compressed data
    std::vector<uint8_t> compressedData(sourceLen);

    // Compress data
    int result{compress2(compressedData.data(), &destLen, reinterpret_cast<const Bytef*>(data.data()), sourceLen, level)};
    if (result != Z_OK) {
        throw std::runtime_error("zlibCompress: compress2 failed with error " + std::to_string(result));
    }

    // Resize memory allocated to compressed data to its actual size
    compressedData.resize(destLen);

    return compressedData;
}

std::vector<uint8_t> zlibTruncateCompress(std::vector<float>& data, const int& bits, const int& level) {
    // Truncate data
    std::vector<float> truncatedData(data.size());
    std::transform(data.begin(), data.end(), truncatedData.begin(), 
                   [bits](float value) { return truncateFloat(value, bits, true); });

    // Compress with zlib
    return zlibCompress(truncatedData, level);
}

template <typename T>
std::vector<T> zlibDecompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) {
    // Allocate memory for decompressed data
    std::vector<T> decompressedData(uncompressedSize);

    // Decompress data
    uLongf destLen{uncompressedSize * sizeof(T)};
    int result{uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &destLen, compressedData.data(), compressedData.size())};
    if (result != Z_OK) {
        throw std::runtime_error("zlibDecompress: uncompress failed with error " + std::to_string(result));
    }

    return decompressedData;
}

// Test functions ---------------------------------------------------------------------------------------------------

CompressionResults timedZlibCompress(std::vector<char> data, const int& level=Z_DEFAULT_COMPRESSION) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{zlibCompress<char>(data, level)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {compressedData, duration};
}

CompressionResults timedZlibTruncateCompress(std::vector<float> data, const int& bits, const int& level) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{zlibTruncateCompress(data, bits, level)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {compressedData, duration};      // This is needlessly pythonic, should be using pass-by-ref instead
}

template<typename T>
DecompressionResults<T> timedZlibDecompress(std::vector<uint8_t> compressedData, const size_t& originalDataSize) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Decompress data
    std::vector<T> decompressedData{zlibDecompress<T>(compressedData, originalDataSize)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {decompressedData, duration};      
}

#endif