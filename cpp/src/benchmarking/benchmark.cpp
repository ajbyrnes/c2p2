#include <format>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "CompressorBenchmark.hpp"
#include "../compressors/TruncCompressor.hpp"
#include "../utils/utils.hpp"
#include "../utils/root.hpp"
#include "../utils/cli.hpp"

void benchmarkBitTruncation(const Args args) {
    // Load benchmark parameters
    std::ifstream f(args.config);
    nlohmann::json config = nlohmann::json::parse(f);

    const int iterations = config.value("iterations", 1);
    const std::vector<int> chunkSizes = config.value("chunkSize", std::vector<int>{});

    const nlohmann::json compressorOptions = config.value("compressorOptions", nlohmann::json::object());

    // Extract specific compressor options with defaults
    const std::vector<int> mantissaBits = compressorOptions.value("mantissaBits", std::vector<int>{});
    const std::vector<int> compressionLevels = compressorOptions.value("compressionLevel", std::vector<int>{});

    // Iterate over parameters
    for (const int chunkSize : chunkSizes) {
        for (const int mb : mantissaBits) {
            for (const int clevel : compressionLevels) {
                std::cout << timeMessage(std::format(
                    "Benchmarking BitTruncation with chunkSize={}, mantissaBits={}, compressionLevel={}",
                    chunkSize, mb, clevel
                    )
                ) << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    const Args args = parseArgs(argc, argv);

    // Print args
    std::cout << "Data file: " << args.data << std::endl;
    std::cout << "Config: " << args.config << std::endl;

    // Read config to figure out which compressor we're benchmarking
    std::string json_filename = args.config;
    std::ifstream f(json_filename);
    nlohmann::json config = nlohmann::json::parse(f);

    std::string compressor = config.value("compressor", "");
    if (compressor == "BitTruncation") {
        benchmarkBitTruncation(args);
    } else {
        std::cerr << "Unsupported compressor: " << compressor << std::endl;
        return 1;
    }

    return 0;
}