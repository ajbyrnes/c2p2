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

CompressionResults timedCompress(std::vector<float> basket) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{szCompress(basket)};

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
    std::vector<float> decompressedData{szDecompress(compressedBasket, originalBasketSize)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {decompressedData, duration};      
}

void printHeader() {
    std::cout << "Distribution,";
    std::cout << "Original Size,";
    std::cout << "Compressed Size,";
    std::cout << "Compression Ratio,";
    std::cout << "Compression Duration (ns),";
    std::cout << "Decompression Duration (ns),";
    std::cout << "Decompressed Mean,";
    std::cout << "Decompressed RMS" << std::endl;
}

void reportResults(const CompressionDecompressionResults results, const std::string& distribution) {
    // Create histogram of decompressed data
    TH1F* h{basketToHistogram(results.decompressedData, std::format("hist_{}", distribution))};
    
    // Write results to stdout
    std::cout << distribution << ",";
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

    // Allocate CompressionDecompressionResults
    CompressionDecompressionResults results{};

    // Compress
    CompressionResults compResult{timedCompress(basket)};
    results.originalSize = basket.size();
    results.compressedSize = compResult.compressedData.size();
    results.compressionDuration = compResult.duration;

    // Decompress
    DecompressionResults decompResult{timedDecompress(compResult.compressedData, results.originalSize)};
    results.decompressedData = decompResult.decompressedData;
    results.decompressionDuration = decompResult.duration;

    // Report results
    reportResults(results, distName);
}

int main() {
    int basketSize{BASKET_SIZE};
    int seed{12345};
    std::string distName{};

    // Uniform distribution
    float min{0};
    float max{static_cast<float>(1 << 5)};
    distName = std::format("uniform_min={}_max={}", min, max);
    std::uniform_real_distribution<float> uniformDistribution(min, max);

    // Test distributions
    // printHeader();
    testDistribution(uniformDistribution, distName, basketSize, seed);
    
        
    return 0;
}