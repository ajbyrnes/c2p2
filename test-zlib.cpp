#include <chrono>
#include <iostream>
#include "compression_lib.hpp"

struct CompressionResults {
    std::vector<uint8_t> compressedData{};
    std::chrono::duration<long, std::nano> duration{};
};

struct DecompressionResults {
    std::vector<float> decompressedData{};
    std::chrono::duration<long, std::nano> duration{};
};

struct CompressionDecompressionResults {
    size_t originalSize{};
    size_t compressedSize{};
    std::chrono::duration<long, std::nano> compressionDuration{};
    std::chrono::duration<long, std::nano> decompressionDuration{};
    std::vector<float> decompressedData{};
};

CompressionResults timedCompress(std::vector<float> basket, const int& bits) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{zlibTruncateCompress(basket, bits)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {compressedData, duration};      // This is needlessly pythonic, should be using pass-by-ref instead
}

DecompressionResults timedDecompress(std::vector<uint8_t> compressedBasket, const size_t& originalBasketSize) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Decompress data
    std::vector<float> decompressedData{zlibDecompress<float>(compressedBasket, originalBasketSize)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {decompressedData, duration};      
}

void printHeader() {
    std::cout << "Distribution,";
    std::cout << "Bits,";
    std::cout << "Original Size,";
    std::cout << "Compressed Size,";
    std::cout << "Compression Ratio,";
    std::cout << "Compression Duration (ns),";
    std::cout << "Decompression Duration (ns),";
    std::cout << "Decompressed Mean,";
    std::cout << "Decompressed RMS" << std::endl;
}

void reportResults(const CompressionDecompressionResults results, const std::string& distribution, const int& bits) {
    // Create histogram of decompressed data
    TH1F* h{basketToHistogram(results.decompressedData, std::format("hist_{}_{}", distribution, bits))};
    
    // Write results to stdout
    std::cout << distribution << ",";
    std::cout << bits << ",";
    std::cout << results.originalSize << ",";
    std::cout << results.compressedSize << ",";
    std::cout << static_cast<double>(results.originalSize) / results.compressedSize << ",";
    std::cout << results.compressionDuration.count() << ",";
    std::cout << results.decompressionDuration.count() << ",";
    std::cout << h->GetMean() << ",";
    std::cout << h->GetRMS() << std::endl;

    // Delete histogram
    delete h;
}

template <typename Distribution>
void testDistribution(const Distribution& distribution, const std::string& distName, const int& basketSize, const int& seed) {
    // Generate basket
    std::vector<float> basket{generateRandomBasket(distribution, seed, basketSize)};

    // Test compression and decompression for each number of bits
    std::vector<CompressionDecompressionResults> results(24);
    for (int bits{0}; bits <= 23; bits++) {
        // Compress
        CompressionResults compResult{timedCompress(basket, bits)};
        results[bits].originalSize = basket.size() * sizeof(float);
        results[bits].compressedSize = compResult.compressedData.size();
        results[bits].compressionDuration = compResult.duration;

        // Decompress
        DecompressionResults decompResult{timedDecompress(compResult.compressedData, results[bits].originalSize)};
        results[bits].decompressedData = decompResult.decompressedData;
        results[bits].decompressionDuration = decompResult.duration;

        // Report results
        reportResults(results[bits], distName, bits);
    }
}

int main() {
    int basketSize{8000};
    int seed{12345};
    std::string distName{};

    // Uniform distribution
    float min{0};
    float max{static_cast<float>(1 << 5)};
    distName = std::format("uniform_min={}_max={}", min, max);
    std::uniform_real_distribution<float> uniformDistribution(min, max);

    // Test distributions
    printHeader();
    testDistribution(uniformDistribution, distName, basketSize, seed);
    
        
    return 0;
}