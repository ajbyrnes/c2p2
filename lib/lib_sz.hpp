#ifndef LIB_SZ_HPP
#define LIB_SZ_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <SZ3/api/sz.hpp>
#include "lib_utils.hpp"

// SZ3 compression -----------------------------------------------------------------------------------------------
// Modified from https://github.com/physnerds/ldrd_compressions/blob/feature/tree/tests/mytest.cxx

CompressorOut szCompress(const CompressorIn& data, const CompressorParams& params) {
    // Configuration
    std::vector<size_t> dims{data.size()};
    SZ3::Config conf({dims[0]});

    conf.errorBoundMode = params.at("errorBoundMode");
    conf.cmprAlgo = params.at("algo");
    conf.interpAlgo = params.at("interpAlgo");
    conf.absErrorBound = 1.0 / std::pow(10, params.at("absErrorBoundExp"));

    // Compress -- allocates memory!
    size_t compressedSize;
    char* compressedData = SZ_compress(conf, data.data(), compressedSize);

    // Wrap result in vector, then free
    std::vector<uint8_t> compressedDataVec(compressedData, compressedData + compressedSize);
    free(compressedData);

    return compressedDataVec;
}

DecompressorOut szDecompress(const DecompressorIn& compressedData, const size_t& uncompressedSize) {
    // Reinterpret compressed data as char*
    const char* compressedDataPtr = reinterpret_cast<const char*>(compressedData.data());

    // Decompress -- allocates memory!
    SZ3::Config conf{};
    float* decompressedDataPtr = nullptr;
    SZ_decompress(conf, compressedDataPtr, compressedData.size(), decompressedDataPtr);

    // Wrap result in vector, then free
    std::vector<float> decompressedVec(decompressedDataPtr, decompressedDataPtr + uncompressedSize);
    free(decompressedDataPtr);

    return decompressedVec;
}

#endif // LIB_SZ_HPP
