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
    std::string outputCSV = std::format("../benchmark results/{}_benchmark-TruncCompressor.csv", getTimestamp(true));
    std::string outputROOT = std::format("../benchmark results/{}_benchmark-TruncCompressor.root", getTimestamp(true));

    std::string treename{"CollectionTree"};
    std::vector<std::string> branches{
        "AnalysisJetsAuxDyn.pt",
        "AnalysisJetsAuxDyn.eta",
        "AnalysisJetsAuxDyn.phi"
    };
    
    for (const std::string& branch : branches) {
        // Read data from input file
        std::vector<float> rawData = readVectorFloatBranchFromFile(
            args.inputFile, treename, branch, args.maxBytes
        );


        // Package data for compressor
        UncompressedData inputData{
            .data = rawData,
            .dataName = branch,
            .fileName =  args.inputFile.substr(args.inputFile.find_last_of("/\\") + 1),
            .dims = {rawData.size()},
            .numFloats = rawData.size(),
        };

        // Iterate over compressor settings
        std::vector<int> mantissaBits{16, 15, 14, 13, 12, 11, 10, 9, 8};
        int compressionLevel{5};
        
        for (size_t i{0}; i < mantissaBits.size(); i++) {
            int bits = mantissaBits[i];
            std::cout << timeMessage(std::format("Running benchmark for compressor '{}' with compression level {} and mantissa bits {}...", 
                                        args.compressor, compressionLevel, bits)) << std::endl;

            // Create compressor
            std::shared_ptr<Compressor> compressor = std::make_shared<TruncCompressor>(compressionLevel, bits);
            std::shared_ptr<TruncCompressor> truncCompressor = std::dynamic_pointer_cast<TruncCompressor>(compressor);

            // Create benchmark instance
            CompressorBenchmark benchmark(compressor, inputData, outputCSV, outputROOT, args.iterations);
            static bool headerWritten = false; // Static variable to ensure header is written only once

            
            // Run benchmark
            std::cout << timeMessage(std::format("Running benchmark for branch '{}' with compression level {} and mantissa bits {}...", 
                                        branch, compressionLevel, bits)) << std::endl;

            benchmark.run(!headerWritten);
            
            if (!headerWritten) {
                headerWritten = true;
            }

            std::cout << timeMessage(std::format("Benchmark completed. Results written to '{}'", outputCSV)) << std::endl;
        }
    }

    return 0;
}