// Read data from ROOT file
// Compress data
// Write decompressed data out to ROOT file

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "CompressorBenchmark.hpp"
#include "../compressors/TruncCompressor.hpp"
#include "../compressors/SZ3Compressor.hpp"
#include "../utils/utils.hpp"

int main(int argc, char* argv[]) {
    // Read parameters from command line arguments
    Args args{parseArgs(argc, argv)};
    std::string filename = std::format("{}-{}", getTimestamp(), args.benchmarkOutputFile);

    // Load data from files
    const std::vector<float> data = readVectorFloatBranchFromFiles(
        dataFiles, args.treename, args.branchname, args.maxBytes
    );

    // Compress data using indicated compressor and params
    std::shared_ptr<Compressor> compressor = nullptr;
    if (args.compressor == "TruncCompressor") {
        compressor = std::make_shared<TruncCompressor>(args.relErrorBound, args.algo);
    }
    else if (args.compressor == "SZ3Compressor") {
        compressor = std::make_shared<SZ3Compressor>(args.relErrorBound, args.algo);
    }
    else {
        std::cerr << "Unknown compressor type: " << args.benchmark << std::endl;
        return 1;
    }

    // Create benchmark instance
    CompressorBenchmark benchmark(compressor, {data, args.branchname}, filename, args.iterations);
    benchmark.writeCSVHeader();

}