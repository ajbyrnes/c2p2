#include <format>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <chrono>

#include "../utils/cli.hpp"
#include "../utils/utils.hpp"
#include "../compressors/TruncCompressor.hpp"

int main(int argc, char* argv[]) {
    // Create compressor
    int compressionLevel{5};
    int mantissaBits{13};

    TruncCompressor compressor{compressionLevel, mantissaBits};

    // Generate random dummy data
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<float> dis(0.0f, 10.0f);
    std::vector<float> data(10000);
    for (auto& val : data) {
        val = dis(gen);
    }

    // Compress and decompress
    CompressedData compressed = compressor.compress(data);
    std::vector<float> decompressed = compressor.decompress(compressed);

    // Print compressor details
    std::cout << "Compressor: " << compressor.toString() << "\n\n";

    // Print length of float and byte vectors
    std::cout << "Length of float vector: " << data.size() << "\n";
    std::cout << "Length of compressed byte vector: " << compressed.data.size() << "\n\n";

    // Print original vs decompressed data side-by-side
    std::cout << std::format("{:<20} {:<20}\n", "Original", "Decompressed");
    for (size_t i = 0; i < 10; ++i) {
        std::cout << std::format("{:<20.10f} {:<20.10f}", data[i], decompressed[i]) << std::endl;
    }

    return 0;
}