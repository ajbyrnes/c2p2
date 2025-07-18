
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "CompressorBenchmark.hpp"
#include "../compressors/SZ3Compressor.hpp"
#include "../utils/utils.hpp"

int main(int argc, char* argv[]) {
    // Read parameters from command line arguments
    Args args{parseArgs(argc, argv)};

    // Parameter setup
    std::string outputCSV = std::format("../benchmark results/{}_benchmark-SZ3Compressor.csv", getTimestamp(true));
    std::string outputROOT = std::format("../benchmark results/{}_benchmark-SZ3Compressor.root", getTimestamp(true));

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

        // Write original data to result tree
        writeFloatVectorToFile(outputROOT, rawData, branch, "original", false);

        // Package data for compressor
        UncompressedData inputData{
            .data = rawData,
            .dataName = branch,
            .fileName = args.inputFile.substr(args.inputFile.find_last_of("/\\") + 1),
            .dims = {rawData.size()},
            .numFloats = rawData.size()
        };

        // Conduct benchmark over different compressor settings
        std::vector<float> errorBounds{5, 1, 1e-2, 1e-4};
        bool useAbsError = true;

        std::vector<int> algorithms{3};     // NOPRED only
        // std::vector<int> algorithms{0, 1, 2, 3};

        for (float errorBound : errorBounds) {
            for (int algo : algorithms) {
                std::cout << timeMessage(std::format("Running benchmark for compressor 'SZ3Compressor' with algorithm {} and error bound {}...", algo, errorBound)) << std::endl;

                // Create compressor
                std::shared_ptr<Compressor> compressor = std::make_shared<SZ3Compressor>(algo, errorBound, useAbsError);
                std::shared_ptr<SZ3Compressor> sz3Compressor = std::dynamic_pointer_cast<SZ3Compressor>(compressor);

                // Create benchmark instance
                CompressorBenchmark benchmark(compressor, inputData, outputCSV, outputROOT, args.iterations);
                static bool headerWritten = false; // Static variable to ensure header is written only once

                std::cout << timeMessage(std::format("Running benchmark for branch '{}' with algorithm {} and error bound {}...", 
                                            branch, algo, errorBound)) << std::endl;

                // Run benchmark
                try {
                    benchmark.run(!headerWritten);
                    if (!headerWritten) {
                        headerWritten = true;
                    }
                } catch (const std::exception& e) {
                    std::cerr << timeMessage(std::format("Error during benchmark: {}", e.what())) << std::endl;
                    continue;
                }

                std::cout << timeMessage(std::format("Benchmark completed. Results written to '{}'", outputCSV)) << std::endl;

                // Destroy compressor
                compressor.reset();
                sz3Compressor.reset();
            }
        }
    }

    return 0;
}