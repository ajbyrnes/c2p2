#include <format>
#include <iostream>
#include <random>
#include <vector>

#include "lib/utils.hpp"
#include "lib/TrunkCompressor.hpp"

int main() {
    // Generate random data
    size_t dataSize{10 * MB / sizeof(float)};
    std::vector<float> data = generateUniformRandomData(dataSize, -1.0f, 1.0f);

    // Generate 10 random indices from (0, dataSize - 1)
    std::vector<size_t> randomIndices(10);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, dataSize - 1);
    for (size_t i = 0; i < randomIndices.size(); ++i) {
        randomIndices[i] = dis(gen);
    }

    // Iterate over precision levels
    for (int precision{7}; precision > 0; --precision) {
        // Create compressor
        TrunkCompressor compressor(precision, 9, false);

        // Compress data
        std::vector<uint8_t> compressedData = compressor.compress(data);

        // Decompress data
        std::vector<float> decompressedData = compressor.decompress(compressedData, dataSize);
    
        // Print 10 random values 
        std::cout << std::format("Precision: {:2}", precision);
        for (size_t i = 0; i < randomIndices.size(); ++i) {
            std::cout << std::format(" {:7f}", decompressedData[randomIndices[i]]);
        }
        std::cout << std::endl;
    }
}