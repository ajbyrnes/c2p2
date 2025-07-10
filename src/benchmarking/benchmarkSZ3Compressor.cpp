#include <format>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "CompressorBenchmark.hpp"
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

    // Dummy data as sanity check
    // Random data sorted in ascending order
    // size_t numMB = 60 * 1024 * 1024; // 10 MB
    // std::vector<float> dummyData(numMB / 8);

    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_real_distribution<float> dis(0.0f, 3.0f);
    // for (size_t i = 0; i < dummyData.size(); ++i) {
    //     dummyData[i] = dis(gen);
    // }

    // std::sort(dummyData.begin(), dummyData.end());

    // std::string dummyDataName = "RandomSorted";

    // Run the benchmark for each combination of compression level and mantissa bits
    // 0 = ALGO_LORENZO_REG
    // 1 = ALGO_INTERP_LORENZO
    // 2 = ALGO_INTERP
    // 3 = ALGO_NOPRED
    // 4 = ALGO_LOSSLESS
    std::vector<int> algorithms{0, 1, 2, 3};
    // As of right now,
    // 5e-4 fails on algo 2 only -- std::bad_alloc
    // 5e-5 fails on algo 0 -- free: bad next
    // 5e-5 fails on algos 1..4 -- std::bad_alloc
    std::vector<float> relErrorBounds{5e-3, 5e-4, 5e-5};

    bool header = false;
    for (float relError : relErrorBounds) {
        for (int algo : algorithms) {
            if (relError == 5e-5 && algo == 0) continue;

            // Create compressor depending on the benchmark type
            std::shared_ptr<Compressor> compressor = std::make_shared<SZ3Compressor>(relError, algo);
            std::shared_ptr<SZ3Compressor> sz3Compressor = std::dynamic_pointer_cast<SZ3Compressor>(compressor);

            // Create benchmark instance
            CompressorBenchmark benchmark(compressor, {data, args.branchname}, filename, args.iterations);
            // CompressorBenchmark benchmark(compressor, {dummyData, dummyDataName}, filename, args.iterations);
            if (!header) {
                benchmark.writeCSVHeader();
                header = true;
            }

            std::cout << std::format("[{}] Running benchmark with algorithm {} and relative error bound {}...", getTimestamp(), algo, relError) << std::endl;
            try {
                benchmark.run();
            } catch (const std::exception& e) {
                std::cerr << std::format("[{}] Error during benchmark: {}", getTimestamp(), e.what()) << std::endl;
                continue; // Skip to next iteration if an error occurs
            }

            // Destroy shared pointers
            compressor.reset();
            sz3Compressor.reset();
        }
    }

    // Print final message
    std::cout << std::format("[{}] Benchmark completed. Results written to '{}'", getTimestamp(), filename) << std::endl;

    return 0;

}