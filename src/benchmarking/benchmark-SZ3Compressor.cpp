
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
    std::string outputCSV = std::format("{}-benchmark-SZ3Compressor.csv", getTimestamp());
    std::string outputROOT = std::format("{}-benchmark-SZ3Compressor.root", getTimestamp());

    std::string treename{"CollectionTree"};
    std::vector<std::string> branches{
        "AnalysisJetsAuxDyn.pt",
        "AnalysisJetsAuxDyn.eta",
        "AnalysisJetsAuxDyn.phi"
    };

    // std::vector<float> relErrorBounds{5e-3};
    std::vector<float> relErrorBounds{5e-2, 5e-3, 5e-4, 5e-5};
    std::vector<int> algorithms{0, 1, 2, 3};

    for (const std::string& branch : branches) {
        // Read data from input file
        std::vector<float> rawData = readVectorFloatBranchFromFile(
            args.inputFile, treename, branch, args.maxBytes
        );

        UncompressedData inputData{
            .data = rawData,
            .dataName = branch,
            .fileName = args.inputFile,
            .dims = {rawData.size()},
            .numFloats = rawData.size()
        };

        for (float relError : relErrorBounds) {
            for (int algo : algorithms) {
                std::cout << timeMessage(std::format("Running benchmark for compressor 'SZ3Compressor' with algorithm {} and relative error bound {}...", algo, relError)) << std::endl;

                // Create compressor
                std::shared_ptr<Compressor> compressor = std::make_shared<SZ3Compressor>(relError, algo);
                std::shared_ptr<SZ3Compressor> sz3Compressor = std::dynamic_pointer_cast<SZ3Compressor>(compressor);

                // Create benchmark instance
                CompressorBenchmark benchmark(compressor, inputData, outputCSV, outputROOT, args.iterations);
                static bool headerWritten = false; // Static variable to ensure header is written only once

                std::cout << timeMessage(std::format("Running benchmark for branch '{}' with algorithm {} and relative error bound {}...", 
                                            branch, algo, relError)) << std::endl;

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