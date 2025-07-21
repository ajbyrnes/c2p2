

#include <iostream>
#include <vector>
#include <format>
#include <cmath>

#include "../benchmarking/CompressorBenchmark.hpp"
#include "../compressors/Compressor.hpp"
#include "../compressors/SZ3Compressor.hpp"
#include "../utils/utils.hpp"
#include "../utils/datagen.hpp"

DecompressedData runBenchmark(const std::shared_ptr<Compressor>& compressor, const UncompressedData& data, const std::string& outputCSV, const std::string& outputROOT, int iterations) {
    CompressorBenchmark benchmark(compressor, data, outputCSV, iterations);
    static bool headerWritten = false; // Static variable to ensure header is written only once

    DecompressedData decompressedData{};

    try {
        decompressedData = benchmark.run(!headerWritten);
        headerWritten = true; // Set to true after the first run
    } catch (const std::exception& e) {
        std::cerr << timeMessage(std::format("Error during benchmark: {}", e.what())) << std::endl;
        return {};
    }

    std::cout << timeMessage(std::format("Benchmark completed. Results written to '{}'", outputCSV)) << std::endl;
    return decompressedData;
}

int main() {
    // Generate multidimensional dummy data
    std::cout << timeMessage("Generating dummy data for SZ3 compressor validation") << std::endl;
    
    size_t n = 1000;
    std::vector<float> circularData2D = data2D(n, n, [n](size_t i, size_t j) {
        float x = indexToCoord(j, n);
        float y = indexToCoord(i, n);
        return x * x + y * y;
    });

    UncompressedData uncompressedCircularData2D{
        .data = circularData2D,
        .dataName = "circularData2D",
        .fileName = "generated",
        .dims = {n, n},
        .numFloats = n * n
    };

    // Generate smoke test data
    std::vector<size_t> smokeDims({100, 200, 300});

    std::vector<float> smokeData(smokeDims[0] * smokeDims[1] * smokeDims[2]);
    std::vector<size_t> stride({smokeDims[1] * smokeDims[2], smokeDims[2], 1});

    for (size_t i = 0; i < smokeDims[0]; ++i) {
        for (size_t j = 0; j < smokeDims[1]; ++j) {
            for (size_t k = 0; k < smokeDims[2]; ++k) {
                double x = static_cast<double>(i) - static_cast<double>(smokeDims[0]) / 2.0;
                double y = static_cast<double>(j) - static_cast<double>(smokeDims[1]) / 2.0;
                double z = static_cast<double>(k) - static_cast<double>(smokeDims[2]) / 2.0;
                smokeData[i * stride[0] + j * stride[1] + k] =
                    static_cast<float>(.0001 * y * sin(y) + .0005 * cos(pow(x, 2) + x) + z);
            }
        }
    }

    UncompressedData uncompressedSmokeData{
        .data = smokeData,
        .dataName = "smokeData",
        .fileName = "generated",
        .dims = smokeDims,
        .numFloats = smokeDims[0] * smokeDims[1] * smokeDims[2]
    };

    // Write original data to ROOT file
    std::string inputFileName = "smokeData_generated";
    std::string outputCSV = std::format("../benchmark results/{}_benchmark-SZ3Compressor_{}.csv", inputFileName, getTimestamp(true));
    std::string outputROOT = std::format("../benchmark results/{}_benchmark-SZ3Compressor_{}.root", inputFileName, getTimestamp(true));

    // Benchmark SZ3 compressor on the generated data
    std::cout << timeMessage("Benchmarking SZ3 compressor") << std::endl;

    // std::vector<float> relErrorBounds{0.1, 0.05, 0.005, 0.0005};      // 10%, 5%, 0.5%, 0.05%
    // std::vector<float> absErrorBounds{10, 5, 1, 0.1}; // 10, 5, 1, 0.1
    std::vector<float> relErrorBounds{};
    std::vector<float> absErrorBounds{1e-4};

    // 0 = ALGO_LORENZO_REG
    // 1 = ALGO_INTERP_LORENZO
    // 2 = ALGO_INTERP
    // 3 = ALGO_NOPRED
    // 4 = ALGO_LOSSLESS
    std::vector<int> algorithms{0, 1, 2, 3};

    // Loop over relative error bounds
    for (const float& errorBound : relErrorBounds) {
        for (int algo : algorithms) {
            std::cout << timeMessage(std::format("Running benchmark for SZ3Compressor with algorithm {} and relative error bound {}...", algo, errorBound)) << std::endl;

            auto compressor = std::make_shared<SZ3Compressor>(algo, errorBound, false);
            auto sz3Compressor = std::dynamic_pointer_cast<SZ3Compressor>(compressor);

            // Benchmark on circular data
            // runBenchmark(compressor, uncompressedCircularData2D, outputCSV, outputROOT, 1);

            // Benchmark on smoke test data
            runBenchmark(compressor, uncompressedSmokeData, outputCSV, outputROOT, 1);
        }
    }

    std::vector<DecompressedData> decompressedSmoke{};

    // Loop over absolute error bounds
    for (const float& errorBound : absErrorBounds) {
        for (int algo : algorithms) {
            std::cout << timeMessage(std::format("Running benchmark for SZ3Compressor with algorithm {} and absolute error bound {}...", algo, errorBound)) << std::endl;

            auto compressor = std::make_shared<SZ3Compressor>(algo, errorBound, true);
            auto sz3Compressor = std::dynamic_pointer_cast<SZ3Compressor>(compressor);

            // Benchmark on circular data
            // runBenchmark(compressor, uncompressedCircularData2D, outputCSV, outputROOT, 1);

            // Benchmark on smoke test data
            decompressedSmoke.push_back(runBenchmark(compressor, uncompressedSmokeData, outputCSV, outputROOT, 1));
        }
    }

    // Write decompressed data to ROOT file
    decompressedSmoke.push_back({
        .data = uncompressedSmokeData.data,
        .compressor = "original",
        .dataName = uncompressedSmokeData.dataName,
        .ogDataFileName = uncompressedSmokeData.fileName
    });

    std::cout << timeMessage("Writing decompressed data to ROOT file") << std::endl;
    writeDecompressedDataToFile(outputROOT, uncompressedSmokeData.dataName, decompressedSmoke);

    return 0;
}