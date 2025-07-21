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
    std::string inputFileName = args.inputFile.substr(args.inputFile.find_last_of("/\\") + 1);
    std::string outputCSV = std::format("../benchmark results/{}_benchmark-TruncCompressor_{}.csv", inputFileName, getTimestamp(true));
    std::string outputROOT = std::format("../benchmark results/{}_benchmark-TruncCompressor_{}.root", inputFileName, getTimestamp(true));

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
            .fileName = inputFileName,
            .dims = {rawData.size()},
            .numFloats = rawData.size(),
        };

        // Iterate over compressor settings
        std::vector<int> mantissaBits{16, 15, 14, 13, 12, 11, 10, 9, 8};
        int compressionLevel{5};

        std::vector<DecompressedData> decompressedBranch{};

        for (size_t i{0}; i < mantissaBits.size(); i++) {
            int bits = mantissaBits[i];
            std::cout << timeMessage(std::format("Running benchmark for compressor '{}' with compression level {} and mantissa bits {}...", 
                                        args.compressor, compressionLevel, bits)) << std::endl;

            // Create compressor
            std::shared_ptr<Compressor> compressor = std::make_shared<TruncCompressor>(compressionLevel, bits);
            std::shared_ptr<TruncCompressor> truncCompressor = std::dynamic_pointer_cast<TruncCompressor>(compressor);

            // Create benchmark instance
            CompressorBenchmark benchmark(compressor, inputData, outputCSV, args.iterations);
            static bool headerWritten = false; // Static variable to ensure header is written only once

            
            // Run benchmark
            std::cout << timeMessage(std::format("Running benchmark for branch '{}' with compression level {} and mantissa bits {}...", 
                                        branch, compressionLevel, bits)) << std::endl;

            decompressedBranch.push_back(benchmark.run(!headerWritten));

            if (!headerWritten) {
                headerWritten = true;
            }

            std::cout << timeMessage(std::format("Benchmark completed. Results written to '{}'", outputCSV)) << std::endl;
        
            // Destroy compressor
            compressor.reset();
            truncCompressor.reset();
        }

        // Write decompressed branch data
        decompressedBranch.push_back({
            .data = inputData.data,
            .compressor = "original",
            .dataName = inputData.dataName,
            .ogDataFileName = inputData.fileName
        });

        std::cout << timeMessage(std::format("Writing decompressed data to '{}'", outputROOT));
        writeDecompressedDataToFile(outputROOT, inputData.dataName, decompressedBranch);
    }

    return 0;
}