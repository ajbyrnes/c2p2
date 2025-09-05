
#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "CompressorBenchmark.hpp"
#include "../compressors/SZ3Compressor.hpp"
#include "../utils/utils.hpp"
#include "../utils/root.hpp"
#include "../utils/cli.hpp"

int main(int argc, char* argv[]) {
    // Read parameters from command line arguments
    Args args{parseArgs(argc, argv)};

    // Parameter setup
    std::string inputFileName = args.inputFile.substr(args.inputFile.find_last_of("/\\") + 1);
    std::string outputCSV = std::format("../benchmark results/{}_benchmark-SZ3Compressor_{}.csv", inputFileName, getTimestamp(true));
    std::string outputROOT = std::format("../benchmark results/{}_benchmark-SZ3Compressor_{}.root", inputFileName, getTimestamp(true));

    std::string treename{"CollectionTree"};
    std::vector<std::string> branches{
        "AnalysisJetsAuxDyn.pt",
        "AnalysisJetsAuxDyn.eta",
        "AnalysisJetsAuxDyn.phi"
    };

    for (const std::string& branch : branches) {
        // Read data from input file
        std::vector<float> rawData = readVectorFloatBranchFromRootFile(
            args.inputFile, treename, branch, args.maxBytes
        );

        // Write original data to result tree
        // writeFloatVectorToFile(outputROOT, rawData, branch, "original");

        // Package data for compressor
        UncompressedData inputData{
            .data = rawData,
            .dataName = branch,
            .fileName = inputFileName,
            .dims = {rawData.size()}, 
            .numFloats = rawData.size()
        };

        // Conduct benchmark over different compressor settings
        std::vector<float> absErrorBounds{
            1, 0.75, 0.5, 0.25, 
            0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001
        };

        // std::vector<float> relErrorBounds{
        //     0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001
        // };
        // std::vector<int> algorithms{0, 1, 2};     // NOPRED only
        std::vector<int> algorithms{0, 1, 2, 3};
        // std::vector<int> algorithms{3};

        std::vector<DecompressedData> decompressedBranch{};

        for (float errorBound : absErrorBounds) {
            for (int algo : algorithms) {
                std::cout << timeMessage(std::format("Running benchmark for compressor 'SZ3Compressor' with algorithm {} and error bound {}...", algo, errorBound)) << std::endl;

                // Create compressor
                std::shared_ptr<Compressor> compressor = std::make_shared<SZ3Compressor>(algo, errorBound, true);
                std::shared_ptr<SZ3Compressor> sz3Compressor = std::dynamic_pointer_cast<SZ3Compressor>(compressor);

                // Create benchmark instance
                CompressorBenchmark benchmark(compressor, inputData, outputCSV, args.iterations);
                static bool headerWritten = false; // Static variable to ensure header is written only once

                std::cout << timeMessage(std::format("Running benchmark for branch '{}' with algorithm {} and error bound {}...", 
                                            branch, algo, errorBound)) << std::endl;

                // Run benchmark
                try {
                    decompressedBranch.push_back(benchmark.run(!headerWritten));
                    // if (algo == 3) {
                    //     decompressedBranch.push_back(benchmark.run(!headerWritten));
                    // }
                    // else {
                    //     benchmark.run(!headerWritten);
                    // }

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

        // Write decompressed branch data
        decompressedBranch.push_back({
            .data = inputData.data,
            .compressor = "original",
            .dataName = inputData.dataName,
            .ogDataFileName = inputData.fileName
        });

        std::cout << timeMessage(std::format("Writing decompressed data to {}", outputROOT));
        writeDecompressedDataToRootFile(outputROOT, inputData.dataName, decompressedBranch);
    }

    return 0;
}