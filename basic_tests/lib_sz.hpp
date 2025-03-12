#include <iostream>
#include <vector>
#include <SZ3/api/sz.hpp>
#include "lib_utils.hpp"

#ifndef LIB_SZ_HPP
#define LIB_SZ_HPP

// SZ3 compression ------------------------------------------------------------------------------------------------
// Modified from https://github.com/physnerds/ldrd_compressions/blob/feature/tree/tests/mytest.cxx

std::vector<uint8_t> szCompress(std::vector<float> data, SZ3::EB errorBoundMode, SZ3::ALGO algo, SZ3::INTERP_ALGO ialgo, float absErrorBound) {
    std::cout << "Compressing data..." << std::endl;
    // Configuration
    SZ3::Config conf({data.size() * sizeof(float)});
    conf.errorBoundMode = errorBoundMode;
    conf.cmprAlgo = algo;
    conf.interpAlgo = ialgo;
    conf.absErrorBound = absErrorBound;

    // Compress
    size_t compressedSize;
    char* compressedData{SZ_compress(conf, data.data(), compressedSize)};

    // Return compressed data
    return std::vector<uint8_t>(compressedData, compressedData + compressedSize);
}

std::vector<float> szDecompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) {
    std::cout << "Decompressing data..." << std::endl;
    // Configuration
    SZ3::Config conf({uncompressedSize});

    // Convert compressed data to char*
    const char* compressedDataPtr{reinterpret_cast<const char*>(compressedData.data())};

    // Allocate memory for decompressed data
    float* decompressedDataPtr{};

    SZ_decompress(conf, compressedDataPtr, compressedData.size(), decompressedDataPtr);

    return std::vector<float>(decompressedDataPtr, decompressedDataPtr + uncompressedSize);
}

// Test functions ---------------------------------------------------------------------------------------------------

CompressionResults timedSzCompress(std::vector<float> data, SZ3::EB errorBoundMode, SZ3::ALGO algo, SZ3::INTERP_ALGO ialgo, float absErrorBound) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{szCompress(data, errorBoundMode, algo, ialgo, absErrorBound)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {compressedData, duration};
}

DecompressionResults<float> timedSzDecompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Decompress data
    std::vector<float> decompressedData{szDecompress(compressedData, uncompressedSize)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {decompressedData, duration};
}

#endif