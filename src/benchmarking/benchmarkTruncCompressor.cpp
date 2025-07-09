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
    std::string filename = std::format("{}-{}.csv", getTimestamp(), args.outputFile);

    // Create compressor depending on the benchmark type
    std::shared_ptr<Compressor> compressor = std::make_shared<TruncCompressor>(args.compressionLevel, args.mantissaBits);
    std::shared_ptr<TruncCompressor> truncCompressor = std::dynamic_pointer_cast<TruncCompressor>(compressor);

    // Load data from files
    std::vector<float> data = readVectorFloatBranchFromFiles(
        dataFiles, args.treename, args.branchname, args.maxBytes
    );

    // Create benchmark instance
    CompressorBenchmark benchmark(compressor, {data, args.branchname}, filename, args.iterations);
    benchmark.writeCSVHeader();

    // Run the benchmark for each combination of compression level and mantissa bits
    std::vector<int> compressionLevels{3, 5, 7, 9};
    std::vector<int> mantissaBits{16, 8};

    for (int bits : mantissaBits) {
        for (int level : compressionLevels) {
            truncCompressor->setCompressionLevel(level);
            truncCompressor->setMantissaBits(bits);

            std::cout << std::format("[{}] Running benchmark with compression level {} and mantissa bits {}...", getTimestamp(), level, bits) << std::endl;
            benchmark.run();
            benchmark.writeLastResultToCSV();
        }
    }

    // Write results to CSV file
    benchmark.writeResultsToCSV();

    // Print final message
    std::cout << std::format("[{}] Benchmark completed. Results written to '{}'", getTimestamp(), filename) << std::endl;

    return 0;

}