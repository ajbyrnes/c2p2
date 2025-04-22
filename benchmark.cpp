#include <iostream>
#include <format>
#include <vector>

#include "lib/utils.hpp"

std::vector<SZ3::ALGO> algos{
    SZ3::ALGO_LORENZO_REG,
    SZ3::ALGO_INTERP_LORENZO,
    SZ3::ALGO_INTERP,
    SZ3::ALGO_NOPRED
};

std::vector<SZ3::INTERP_ALGO> interpAlgos{
    SZ3::INTERP_ALGO_LINEAR,
    SZ3::INTERP_ALGO_CUBIC
};

std::ofstream& openCSV(std::string filename) {
    std::ofstream& file{*(new std::ofstream(filename))};
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Write header
    file << "Compresser,";
    file << "Precision,";
    file << "CompressionLevel,";
    file << "ErrorBoundMode,";
    file << "Algo,";
    file << "InterpAlgo,";
    file << "CompressionTimeMS,";
    file << "DecompressionTimeMS,";
    file << "CompressionRatio,";
    file << "CompressedDataSize,";
    file << "OriginalDataSize";
    file << std::endl;
    return file;
}

int main(int argc, char* argv[]) {
    // Set parameters
    BenchmarkParams params{parseArguments(argc, argv)};

    // Open file for writing
    std::string filename{
        std::format("SZ3Sweep-{}-{}.csv",
                    getHost(), timestamp())
    };

    std::ofstream& file{openCSV(filename)};

    // Iterate over SZ3 params
    for (const SZ3::ALGO& algo : algos) {
        for (const SZ3::INTERP_ALGO interpAlgo : interpAlgos) {
            params.szAlgo = algo;
            params.szInterpAlgo = interpAlgo;
            
            file << benchmark(params);
        }
    }

    // Close file
    file.close();
}