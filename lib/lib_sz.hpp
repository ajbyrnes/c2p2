#ifndef LIB_SZ_HPP
#define LIB_SZ_HPP

#include <iostream>
#include <vector>
#include <cmath>

#include <SZ3/api/sz.hpp>

#include "lib_utils.hpp"
#include "CompressorBenchmark.hpp"

// SZ3 compression -----------------------------------------------------------------------------------------------
// Modified from https://github.com/physnerds/ldrd_compressions/blob/feature/tree/tests/mytest.cxx

CompressorOut szCompress(const CompressorIn& data, const CompressorParams& params) {
    // Configuration
    std::vector<size_t> dims{data.size()};
    SZ3::Config conf({dims[0]});
    conf.dataType = SZ_FLOAT;

    conf.errorBoundMode = params.at("errorBoundMode");
    conf.relErrorBound = params.at("relativeError");

    // Set parameters only if keys exist
    conf.cmprAlgo = params.at("algo");
    conf.interpAlgo = params.at("interpAlgo");
    
    // Compress -- allocates memory!
    size_t compressedSize;
    char* compressedData = SZ_compress(conf, data.data(), compressedSize);

    // Wrap result in vector, then free
    std::vector<uint8_t> compressedDataVec(compressedData, compressedData + compressedSize);
    free(compressedData);

    return compressedDataVec;
}

CompressorOut szCompressDefault(const CompressorIn& data, const CompressorParams& params) {
    // Configuration
    std::vector<size_t> dims{data.size()};
    SZ3::Config conf({dims[0]});
    conf.dataType = SZ_FLOAT;

    conf.errorBoundMode = params.at("errorBoundMode");
    conf.relErrorBound = params.at("relativeError");

    // conf.cmprAlgo = params.at("algo");
    // conf.interpAlgo = params.at("interpAlgo");
    
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

    // std::vector<size_t> dims{uncompressedSize};
    // SZ3::Config conf({dims[0]});
    // conf.dataType = SZ_FLOAT;

    // conf.errorBoundMode = params.at("errorBoundMode");
    // conf.relErrorBound = params.at("relativeError");
    // conf.cmprAlgo = params.at("algo");
    // conf.interpAlgo = params.at("interpAlgo");
    
    SZ3::Config conf{};

    // Decompress -- allocates memory!
    float* decompressedDataPtr = nullptr;
    SZ_decompress(conf, compressedDataPtr, compressedData.size() * sizeof(float), decompressedDataPtr);

    // Wrap result in vector, then free
    std::vector<float> decompressedVec(decompressedDataPtr, decompressedDataPtr + uncompressedSize);
    free(decompressedDataPtr);

    return decompressedVec;
}

// Precision  ---------------------------------------------------------------------------------

double calculateRelativeError(int precision) {
    return 0.5 * std::pow(10, -precision);
}

// SZ3 parameter sweep -------------------------------------------------------------------------------------------

void paramSweepSZ3(const CompressorIn& data, int precision, int test, int iterations, float noise=-1) {
    // Make test name
    std::string testname{"paramSweep_sz3_" + getTestName(test)};

    // Open CSV to write results
    CompressorParams params{
        {"relativeError", 0},
        {"errorBoundMode", SZ3::EB_REL},
        {"algo", 0},
        {"interpAlgo", 0}
    };

    if (test == 4 || test == 5) {
        params["noise"] = noise;
    }

    params["relativeError"] = calculateRelativeError(precision);

    std::ofstream file = openCSV(testname, params);

    // Compress data with different parameters
    std::vector<SZ3::ALGO> algorithms = {
        SZ3::ALGO_LORENZO_REG,
        SZ3::ALGO_INTERP_LORENZO,
        SZ3::ALGO_INTERP,
        SZ3::ALGO_NOPRED,
        SZ3::ALGO_LOSSLESS
    };

    std::vector<SZ3::INTERP_ALGO> interpAlgorithms = {
        SZ3::INTERP_ALGO_LINEAR,
        SZ3::INTERP_ALGO_CUBIC
    };

    for (const auto& algo : algorithms) {
        params["algo"] = algo;
        for (const auto& interpAlgo : interpAlgorithms) {
            params["interpAlgo"] = interpAlgo;

            // Compress data
            CompressorOut compressedData = szCompress(data, params);
            DecompressorOut decompressedData = szDecompress(compressedData, data.size());
       
            // Run benchmark
            CompressorBenchmark benchmark(
                testname,
                iterations,
                szCompress,
                szDecompress,
                params,
                data
            );

            benchmark.runBenchmark();
            benchmark.writeCSVRow(file);
        }
    }

    // Close file
    file.close();
}

#endif // LIB_SZ_HPP
