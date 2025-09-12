#include <format>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../utils/cli.hpp"
#include "../utils/utils.hpp"
#include "../compressors/TruncCompressor.hpp"

int main(int argc, char* argv[]) {
    // Read parameters from command line arguments
    Args args{parseArgs(argc, argv)};

    // Create compressor
    TruncCompressor compressor(args.compressionLevel, args.mantissaBits);

    // Generate random floats
    std::vector<float> data(10);
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(100.0f);
    for (float& f : data) {
        f = dist(rng);
    }

    CompressedData compressed = compressor.compress({.data = data, .dataName = "test_data"});

    DecompressedData decompressed = compressor.decompress(compressed);

    // Print compressor details
    std::cout << "Compressor: " << compressor.toString() << "\n\n";

    // Print length of float and byte vectors
    std::cout << "Length of float vector: " << data.size() << "\n";
    std::cout << "Length of compressed byte vector: " << compressed.data.size() << "\n\n";

    // Print original vs decompressed data side-by-side
    std::cout << std::format("{:<20} {:<20}\n", "Original", "Decompressed");
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << std::format("{:<20.10f} {:<20.10f}", data[i], decompressed.data[i]) << std::endl;
    }

    return 0;
}