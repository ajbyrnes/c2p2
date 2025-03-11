#include <cstdint>
#include <vector>
#include <zlib.h>

#include "lib_utils.hpp"

#ifndef LIB_ZLIB_HPP
#define LIB_ZLIB_HPP

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

std::vector<uint8_t> zlibTruncateCompress(const std::vector<float>& data, 
                                          const int& bits, 
                                          const int& level=Z_DEFAULT_COMPRESSION);

float truncate(float value, const int& bits, const bool& round=true);
std::vector<float> truncateVectorData(const std::vector<float>& data, 
                                      const int& bits, bool round=true);

// Test functions ---------------------------------------------------------------------------------------------------

CompressionResults timedZlibCompress(std::vector<char> data, const int& level);
CompressionResults timedZlibTruncateCompress(std::vector<float> data, const int& bits, const int& level=Z_DEFAULT_COMPRESSION);

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