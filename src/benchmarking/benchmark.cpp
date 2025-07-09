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
    std::shared_ptr<Compressor> compressor = nullptr;
    if (args.benchmark == "TruncCompressor") {
        compressor = std::make_shared<TruncCompressor>(args.compressionLevel, args.mantissaBits);
    } else {
        std::cerr << "Unknown benchmark: " << args.benchmark << "\n";
        return 1;
    }

    // Load data from files
    std::vector<float> data = readVectorFloatBranchFromFiles(
        dataFiles, args.treename, args.branchname, args.maxBytes
    );

    // Run the benchmark
    CompressorBenchmark benchmark(compressor, {data, args.branchname}, args.iterations);
    benchmark.run();
    benchmark.writeResultsToCSV(std::format("{}-benchmark.csv", getTimestamp()));

    return 0;

}