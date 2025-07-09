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

    // Create compressor depending on the benchmark type
    std::shared_ptr<Compressor> compressor = std::make_shared<TruncCompressor>(args.compressionLevel, args.mantissaBits);
    std::shared_ptr<TruncCompressor> truncCompressor = std::dynamic_pointer_cast<TruncCompressor>(compressor);

    // Load data from files
    std::vector<float> data = readVectorFloatBranchFromFiles(
        dataFiles, args.treename, args.branchname, args.maxBytes
    );

    // Create benchmark instance
    CompressorBenchmark benchmark(compressor, {data, args.branchname}, args.iterations);

    // Run the benchmark for each combination of compression level and mantissa bits
    std::vector<int> compressionLevels{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> mantissaBits{8, 16, 20};

    for (int level : compressionLevels) {
        for (int bits : mantissaBits) {
            truncCompressor->setCompressionLevel(level);
            truncCompressor->setMantissaBits(bits);

            std::cout << std::format("[{}] Running benchmark with compression level {} and mantissa bits {}...", getTimestamp(), level, bits) << std::endl;
            benchmark.run();
            std::cout << " done." << std::endl;
        }
    }

    // Write results to CSV file
    std::string filename = std::format("{}-{}.csv", getTimestamp(), args.outputFile);
    benchmark.writeResultsToCSV(filename);

    // Print final message
    std::cout << std::format("[{}] Benchmark completed. Results written to '{}'", getTimestamp(), filename) << std::endl;

    return 0;

}