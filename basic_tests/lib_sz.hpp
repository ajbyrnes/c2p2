#include <vector>
#include <SZ3/api/sz.hpp>

#include "lib_utils.hpp"

#ifndef LIB_SZ_HPP
#define LIB_SZ_HPP

// #include "lib_utils.hpp"

// CompressionResults timedSzCompress(std::vector<float> data, 
//     SZ3::EB errorBoundMode=SZ3::EB_ABS,
//     SZ3::ALGO algo=SZ3::ALGO_INTERP_LORENZO,
//     SZ3::INTERP_ALGO ialgo=SZ3::INTERP_ALGO_LINEAR,
//     float absErrorBound=1E-3);

// DecompressionResults<float> timedSzDecompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize);

// std::vector<uint8_t> szCompress(std::vector<float> data, 
//     SZ3::EB errorBoundMode=SZ3::EB_ABS,
//     SZ3::ALGO algo=SZ3::ALGO_INTERP_LORENZO,
//     SZ3::INTERP_ALGO ialgo=SZ3::INTERP_ALGO_LINEAR,
//     float absErrorBound=1E-3);

// std::vector<float> szDecompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize);

void helloWorld();

#endif