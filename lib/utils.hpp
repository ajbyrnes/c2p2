#ifndef LIB_UTILS_HPP
#define LIB_UTILS_HPP

#include <chrono>
#include <format>
#include <random>
#include <vector>
#include <unistd.h>

#include "CompressorBench.hpp"

// Constants -----------------------------------------------------------------------------------------------------

constexpr size_t KB{1'000};
constexpr size_t MB{1'000'000};

// Data generation ----------------------------------------------------------------------------------
std::vector<float> generateUniformRandomData(size_t size, float min, float max) {
    std::vector<float> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);

    for (size_t i = 0; i < size; ++i) {
        data[i] = dis(gen);
    }

    return data;
}
std::vector<float> generateGaussianRandomData(size_t size, float mean, float stddev, int seed) {
    std::vector<float> data(size);
    std::random_device rd{};
    std::mt19937 gen(seed);
    std::normal_distribution<float> dis(mean, stddev);

    for (size_t i = 0; i < size; ++i) {
        data[i] = dis(gen);
    }

    return data;
}

// Other utilities ---------------------------------------------------------------------------------------------------
std::string getHost() {
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    return std::string(hostname);
}

std::string timestamp() {
    return std::format("{}", std::chrono::system_clock::now().time_since_epoch().count());
}

// Benchmarking ------------------------------------------------------------------------------------------------------

struct BenchmarkParams {
    double dataMB;

    std::string dataSource;
    int seed;
    float mean;
    float stddev;

    int iterations;
    int precision;
    bool debug;

    int trunkCompressionLevel;

    int szErrorBoundMode;
    int szAlgo;
    int szInterpAlgo;
};

BenchmarkParams parseArguments(int argc, char* argv[]) {
    BenchmarkParams params;

    params.iterations = 5;
    params.precision = 3;
    params.debug = false;

    params.dataMB = 10;
    params.dataSource = "normal";
    params.seed = 12345;
    params.mean = 0.0;
    params.stddev = 1.0f;

    params.trunkCompressionLevel = 9;
    params.szErrorBoundMode = SZ3::EB_REL;
    params.szAlgo = SZ3::ALGO_LORENZO_REG;
    params.szInterpAlgo = SZ3::INTERP_ALGO_LINEAR;

    // Read parameters
    for (int i{1}; i < argc; ++i) {
        std::string arg{argv[i]};
        if (arg == "--iterations") {
            params.iterations = std::stoi(argv[++i]);
        } else if (arg == "--precision") {
            params.precision = std::stoi(argv[++i]);
        } else if (arg == "--debug") {
            params.debug = std::stoi(argv[++i]);
        } else if (arg == "--dataMB") {
            params.dataMB = std::stod(argv[++i]);
        } else if (arg == "--dataSource") {
            params.dataSource = argv[++i];
        } else if (arg == "--mean") {
            params.mean = std::stof(argv[++i]);
        } else if (arg == "--stddev") {
            params.stddev = std::stof(argv[++i]);
        } else if (arg == "--trunkCompressionLevel") {
            params.trunkCompressionLevel = std::stoi(argv[++i]);
        } else if (arg == "--szErrorBoundMode") {
            params.szErrorBoundMode = std::stoi(argv[++i]);
        } else if (arg == "--szAlgo") {
            params.szAlgo = std::stoi(argv[++i]);
        } else if (arg == "--szInterpAlgo") {
            params.szInterpAlgo = std::stoi(argv[++i]);
        } else if (arg == "--seed") {
            params.seed = std::stoi(argv[++i]);
        }
    }

    return params;
}

std::string benchmark(BenchmarkParams params) {
    // Generate random data
    size_t dataSize{static_cast<size_t>(params.dataMB * static_cast<double>(MB)) / sizeof(float)};
    std::vector<float> data{};
    if (params.dataSource == "normal") {
        data = generateGaussianRandomData(dataSize, params.mean, params.stddev, params.seed);
    }

    // Run compression benchmarks
    CompressorBench bench(
        params.iterations, params.dataSource, params.precision, params.trunkCompressionLevel,
        params.szErrorBoundMode, params.szAlgo, params.szInterpAlgo,
        params.debug
    );

    bench.run(data, dataSize);

    // Print results
    for (int compressor{CompressorBench::TRUNK}; compressor <= CompressorBench::SZ; ++compressor) {
        CompressorBench::COMPRESSOR compressorEnum{static_cast<CompressorBench::COMPRESSOR>(compressor)};
        std::cout << "Compressor: " << (compressor == CompressorBench::TRUNK ? "Trunk" : "SZ") << std::endl;
        std::cout << "Compression time: " << bench.getCompressionTime(compressorEnum) << " ms" << std::endl;
        std::cout << "Decompression time: " << bench.getDecompressionTime(compressorEnum) << " ms" << std::endl;
        std::cout << "Compression ratio: " << bench.getCompressionRatio(compressorEnum) << std::endl;
        std::cout << "Compressed data size: " << bench.getCompressedDataSize(compressorEnum) / KB << " KB" << std::endl;
        std::cout << "Original data size: " << bench.getOriginalDataSize() / KB << " KB" << std::endl;
    }

    // Print report
    return bench.generateReport();
}

#endif