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
    std::string filename = std::format("{}-{}", getTimestamp(), args.benchmarkOutputFile);

    // Load data from files
    std::vector<float> data = readVectorFloatBranchFromFiles(
        dataFiles, args.treename, args.branchname, args.maxBytes
    );

    // Run the benchmark for each combination of compression level and mantissa bits
    std::vector<int> compressionLevels{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> mantissaBits{16, 12, 8};

    bool header = false;
    for (int bits : mantissaBits) {
        for (int level : compressionLevels) {
            // Create compressor 
            std::shared_ptr<Compressor> compressor = std::make_shared<TruncCompressor>(level, bits);
            std::shared_ptr<TruncCompressor> truncCompressor = std::dynamic_pointer_cast<TruncCompressor>(compressor);

            // Create benchmark instance
            CompressorBenchmark benchmark(compressor, {data, args.branchname}, filename, args.iterations);
            if (!header) {
                benchmark.writeCSVHeader();
                header = true;
            }

            std::cout << std::format("[{}] Running benchmark with compression level {} and mantissa bits {}...", getTimestamp(), level, bits) << std::endl;
            benchmark.run();
        }
    }

    // Print final message
    std::cout << std::format("[{}] Benchmark completed. Results written to '{}'", getTimestamp(), filename) << std::endl;

    return 0;

}