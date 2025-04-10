#include <iostream>
#include <iomanip>
#include <random>

#include "lib/lib_sz.hpp"

int main(int argc, char* argv[]) {
    // Default parameters
    double MIN{0.0};
    double MAX{1.0};
    int PRECISION{3};

    // Read command line arguments
    for (int i{1}; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-min") {
            MIN = std::stod(argv[++i]);
        } else if (arg == "-max") {
            MAX = std::stod(argv[++i]);
        } else if (arg == "-precision") {
            PRECISION = std::stoi(argv[++i]);
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Usage: " << argv[0] << " [-min min] [-max max] [-precision precision]" << std::endl;
            return 1;
        }
    }

    double RELATIVE_ERROR{calculateRelativeError(PRECISION)}; // Number of bits to truncate

    // Print arguments
    std::cout << "Min: " << MIN << ", Max: " << MAX << std::endl;
    std::cout << "Relative Error: " << RELATIVE_ERROR << std::endl;
    std::cout << "Expected Precision: " << PRECISION << std::endl;

    // Generate 1 MB of random data
    int seed{0};
    size_t dataSize{MB / sizeof(float)};
    std::vector<float> data{generateUniformRandomData(dataSize, MIN, MAX, seed)};

    // Choose 10 random indices in the range [0, dataSize)
    std::mt19937 gen(seed);
    std::uniform_int_distribution<size_t> dist(0, dataSize - 1);
    std::vector<size_t> randomIndices(10);
    for (size_t i = 0; i < randomIndices.size(); ++i) {
        randomIndices[i] = dist(gen);
    }

    // Print data at the random indices
    std::cout << "Before compression: ";
    for (const int& index : randomIndices) {
        std::cout << std::setprecision(PRECISION) << data[index] << "  ";
    }
    std::cout << std::endl;

    CompressorParams params{
        {"relativeError", RELATIVE_ERROR},
        {"errorBoundMode", SZ3::EB_REL}
    };

    // Compress data without choosing parameters
    CompressorOut compressedData = szCompressDefault(data, params);
    DecompressorOut decompressedData = szDecompress(compressedData, data.size());

    // Print data at the random indices
    std::cout << "After compression: (default): ";
    for (const int& index : randomIndices) {
        std::cout << std::setprecision(PRECISION) << decompressedData[index] << "  ";
    }
    std::cout << std::endl;

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

            // Print data at the random indices
            std::cout << "After compression: (algo " << SZ3::enum2Str(algo) << ", interp " << SZ3::enum2Str(interpAlgo) << "): ";
            for (const int& index : randomIndices) {
                std::cout << std::setprecision(PRECISION) << decompressedData[index] << "  ";
            }
            std::cout << std::endl;
        }
    }
}