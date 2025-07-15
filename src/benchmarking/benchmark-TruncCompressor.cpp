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

    std::vector<int> mantissaBits{16, 15, 14, 13, 12, 11, 10, 9, 8};
    int compressionLevel{5};
    
    for (const std::string& branch : branches) {
        // Read data from input file
        std::vector<float> data = readVectorFloatBranchFromFile(
            args.inputFile, treename, branch, args.maxBytes
        );

        for (size_t i{0}; i < mantissaBits.size(); i++) {
            int bits = mantissaBits[i];
            std::cout << timeMessage(std::format("Running benchmark for compressor '{}' with compression level {} and mantissa bits {}...", 
                                        args.compressor, compressionLevel, bits)) << std::endl;

            // Create compressor
            std::shared_ptr<Compressor> compressor = std::make_shared<TruncCompressor>(compressionLevel, bits);
            std::shared_ptr<TruncCompressor> truncCompressor = std::dynamic_pointer_cast<TruncCompressor>(compressor);

            // Create benchmark instance
            CompressorBenchmark benchmark(compressor, {.data = data, .dataName = branch, .fileName = args.inputFile}, outputFilename, args.iterations);
            static bool header = false; // Static variable to ensure header is written only once

            if (!header) {
                benchmark.writeCSVHeader();
                header = true;
            }

            std::cout << timeMessage(std::format("Running benchmark for branch '{}' with compression level {} and mantissa bits {}...", 
                                        branch, compressionLevel, bits)) << std::endl;
            benchmark.run();
            std::cout << timeMessage(std::format("Benchmark completed. Results written to '{}'", outputFilename)) << std::endl;
        }
    }

    return 0;
}