#include <chrono>
#include <fstream>
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
    float originalMean{};
    float originalMeanError{};
    float originalRMS{};
    float originalRMSError{};
    size_t compressedSize{};
    float decompressedMean{};
    float decompressedMeanError{};
    float decompressedRMS{};
    float decompressedRMSError{};
    float meanCE{};
    float meanErrorCE{};
    float rmsCE{};
    float rmsErrorCE{};
    std::chrono::duration<long, std::nano> compressionDuration{};
    std::chrono::duration<long, std::nano> decompressionDuration{};
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

// Write header to file
void writeHeader(std::ofstream& file) {
    // Write header to file
    file << "Distribution,";
    file << "Bits Truncated,";
    file << "Compression Duration (ns),";
    file << "Decompression Duration (ns),";
    file << "Original Size,";
    file << "Compressed Size,";
    file << "Compression Ratio,";
    file << "Original Mean,";
    file << "Original Mean Error,";
    file << "Original RMS,";
    file << "Original RMS Error,";
    file << "Decompressed Mean,";
    file << "Decompressed Mean Error,";
    file << "Decompressed RMS,";
    file << "Decompressed RMS Error,";
    file << "Mean CE,";
    file << "Mean Error CE,";
    file << "RMS CE,";
    file << "RMS Error CE" << std::endl;
}

void writeResults(std::ofstream& file, const CompressionDecompressionResults& results, const std::string& distribution, const int& bits) {
    file << std::format("{}," , distribution);
    file << std::format("{}," , bits);
    file << std::format("{}," , results.compressionDuration.count());
    file << std::format("{}," , results.decompressionDuration.count());
    file << std::format("{}," , results.originalSize);
    file << std::format("{}," , results.compressedSize);
    file << std::format("{}," , static_cast<double>(results.originalSize) / results.compressedSize);
    file << std::format("{}," , results.originalMean);
    file << std::format("{}," , results.originalMeanError);
    file << std::format("{}," , results.originalRMS);
    file << std::format("{}," , results.originalRMSError);
    file << std::format("{}," , results.decompressedMean);
    file << std::format("{}," , results.decompressedMeanError);
    file << std::format("{}," , results.decompressedRMS);
    file << std::format("{}," , results.decompressedRMSError);
    file << std::format("{}," , results.meanCE);
    file << std::format("{}," , results.meanErrorCE);
    file << std::format("{}," , results.rmsCE);
    file << std::format("{}" , results.rmsErrorCE);
    file << std::endl;
}

void printLog(const CompressionDecompressionResults& results, const std::string& distribution, const int& bits) {
    // Print log to console
    std::cout << "Distribution: " << distribution << std::endl;
    std::cout << "Bits Truncated: " << bits << std::endl;
    std::cout << "Compression Duration (ns): " << results.compressionDuration.count() << std::endl;
    std::cout << "Decompression Duration (ns): " << results.decompressionDuration.count() << std::endl;
    std::cout << "Original Size: " << results.originalSize << std::endl;
    std::cout << "Compressed Size: " << results.compressedSize << std::endl;
    std::cout << "Compression Ratio: " << static_cast<double>(results.originalSize) / results.compressedSize << std::endl;
    std::cout << "Original Mean: " << results.originalMean << std::endl;
    std::cout << "==========" << std::endl;
}

template <typename Distribution>
void testDistribution(std::ofstream& file, const Distribution& distribution, const std::string& distName, const int& basketSize, const int& seed) {
    // Generate basket
    std::cout << "(" << std::chrono::system_clock::now() << ") ";
    std::cout << "Generating basket with " << basketSize << " elements for " << distName << std::endl;

    std::vector<float> basket{generateRandomBasket(distribution, seed, basketSize)};

    // Collect original distribution stats
    CompressionDecompressionResults results{};
    TH1F* h{basketToHistogram(basket, distName)};
    results.originalSize = basket.size();
    results.originalMean = h->GetMean();
    results.originalMeanError = h->GetMeanError();
    results.originalRMS = h->GetRMS();
    results.originalRMSError = h->GetRMSError();
    delete h;

    // Test compression and decompression for each number of bits
    for (int bits{0}; bits <= 23; bits++) {
        // Compress
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Compressing with " << bits << "-bit truncation" << std::endl;

        CompressionResults compResult{timedCompress(basket, bits)};
        results.originalSize = basket.size();
        results.compressedSize = compResult.compressedData.size();
        results.compressionDuration = compResult.duration;

        // Decompress
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Decompressing..." << std::endl;

        DecompressionResults decompResult{timedDecompress(compResult.compressedData, results.originalSize)};
        results.decompressionDuration = decompResult.duration;

        // Collect decompressed distribution stats
        TH1F* h{basketToHistogram(decompResult.decompressedData, std::format("hist_{}_{}", distName, bits))};

        results.decompressedMean = h->GetMean();
        results.decompressedMeanError = h->GetMeanError();
        results.decompressedRMS = h->GetRMS();
        results.decompressedRMSError = h->GetRMSError();
        
        results.meanCE = results.originalMean - results.decompressedMean;
        results.meanErrorCE = results.originalMeanError + results.decompressedMeanError;
        results.rmsCE = results.originalRMS - results.decompressedRMS;
        results.rmsErrorCE = results.originalRMSError + results.decompressedRMSError;
        
        delete h;

        // Calculate compression ratio
        double compressionRatio{static_cast<double>(results.originalSize) / results.compressedSize};

        // Write results to file
        writeResults(file, results, distName, bits);
    }
}

int main(int argc, char* argv[]) {
    // Get filename from args, otherwise use default
    std::string filename{};
    if (argc > 1) {
        filename = argv[1];
    } else {
        filename = "results.csv";
    }
    std::cout << "Using filename: " << filename << std::endl;

    int basketSize{BASKET_SIZE};
    int seed{12345};
    std::string distName{};

    // Uniform distribution
    float min{0};
    float max{static_cast<float>(1 << 5)};
    distName = std::format("uniform_min={}_max={}", min, max);
    std::uniform_real_distribution<float> uniformDistribution(min, max);

    // Normal distribution
    

    // Open file for writing
    std::ofstream file{filename};
    writeHeader(file);

    // Test distributions
    testDistribution(file, uniformDistribution, distName, basketSize, seed);

    // Close file
    file.close();
        
    return 0;
}