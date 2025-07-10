#include <format>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "CompressorBenchmark.hpp"
#include "../compressors/TruncCompressor.hpp"
#include "../utils/utils.hpp"

int main(int argc, char* argv[]) {
    // Read parameters from command line arguments
    Args args{parseArgs(argc, argv)};

    // Parameter setup
    std::string outputFilename = std::format("{}-{}", getTimestamp(), args.benchmarkOutputFile);

    std::string treename{"CollectionTree"};
    std::vector<std::string> branches{
        "AnalysisJetsAuxDyn.pt",
        "AnalysisJetsAuxDyn.eta",
        "AnalysisJetsAuxDyn.phi"
    };

    std::vector<int> mantissaBits{16, 12, 8};
    int compressionLevel{5};
    
    for (size_t i{0}; i < mantissaBits.size(); i++) {
        int bits = mantissaBits[i];
        std::cout << std::format("[{}] Running benchmark for compressor '{}' with compression level {} and mantissa bits {}...", 
                                 getTimestamp(), args.compressor, compressionLevel, bits) << std::endl;

        // Create compressor
        std::shared_ptr<Compressor> compressor = std::make_shared<TruncCompressor>(compressionLevel, bits);
        std::shared_ptr<TruncCompressor> truncCompressor = std::dynamic_pointer_cast<TruncCompressor>(compressor);

        for (std::string branch : branches) {
            // Read data from input file
            std::vector<float> data = readVectorFloatBranchFromFile(
                args.inputFile, treename, branch, args.maxBytes
            );

            // Create benchmark instance
            CompressorBenchmark benchmark(compressor, {.data = data, .dataName = branch, .fileName = args.inputFile}, outputFilename, args.iterations);
            static bool header = false; // Static variable to ensure header is written only once

            if (!header) {
                benchmark.writeCSVHeader();
                header = true;
            }

            std::cout << std::format("[{}] Running benchmark for branch '{}' with compression level {} and mantissa bits {}...", 
                                     getTimestamp(), branch, compressionLevel, bits) << std::endl;
            benchmark.run();
        }

        std::cout << std::format("[{}] Benchmark completed. Results written to '{}'\n", getTimestamp(), outputFilename) << std::endl;
    }

    return 0;
}