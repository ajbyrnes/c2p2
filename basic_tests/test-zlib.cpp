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

CompressionResults timedCompress(std::vector<float> basket, const int& bits, const int& level=Z_DEFAULT_COMPRESSION) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{zlibTruncateCompress(basket, bits, level)};

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
    file << "Compression Level,";
    file << "Trial,";
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

void writeResults(std::ofstream& file, const CompressionDecompressionResults& results, const std::string& mode, const int& bits, const int& level, const int& trial) {
    file << std::format("{},", mode);
    file << std::format("{},", bits);
    file << std::format("{},", level);
    file << std::format("{},", trial);
    file << std::format("{},", results.compressionDuration.count());
    file << std::format("{},", results.decompressionDuration.count());
    file << std::format("{},", results.originalSize);
    file << std::format("{},", results.compressedSize);
    file << std::format("{:.16f},", static_cast<double>(results.originalSize) / results.compressedSize);
    file << std::format("{:.16f},", results.originalMean);
    file << std::format("{:.16f},", results.originalMeanError);
    file << std::format("{:.16f},", results.originalRMS);
    file << std::format("{:.16f},", results.originalRMSError);
    file << std::format("{:.16f},", results.decompressedMean);
    file << std::format("{:.16f},", results.decompressedMeanError);
    file << std::format("{:.16f},", results.decompressedRMS);
    file << std::format("{:.16f},", results.decompressedRMSError);
    file << std::format("{:.16f},", results.meanCE);
    file << std::format("{:.16f},", results.meanErrorCE);
    file << std::format("{:.16f},", results.rmsCE);
    file << std::format("{:.16f}", results.rmsErrorCE);
    file << std::endl;
}

void printLog(const CompressionDecompressionResults& results, const std::string& mode, const int& bits, const int& level) {
    // Print log to console
    std::cout << "Mode: " << mode << std::endl;
    std::cout << "Bits Truncated: " << bits << std::endl;
    std::cout << "Compression Level: " << level << std::endl;
    std::cout << "Compression Duration (ns): " << results.compressionDuration.count() << std::endl;
    std::cout << "Decompression Duration (ns): " << results.decompressionDuration.count() << std::endl;
    std::cout << "Original Size: " << results.originalSize << std::endl;
    std::cout << "Compressed Size: " << results.compressedSize << std::endl;
    std::cout << "Compression Ratio: " << static_cast<double>(results.originalSize) / results.compressedSize << std::endl;
    std::cout << "Original Mean: " << results.originalMean << std::endl;
    std::cout << "==========" << std::endl;
}


template <typename Distribution>
std::vector<float> generateRandomTestData(const Distribution& distribution, const std::string& distName, const int& seed, const int& dataSize, const bool& sorted) {
    // Generate basket
    std::cout << "(" << std::chrono::system_clock::now() << ") ";
    std::cout << "Generating vector with " << dataSize << " floats (" << (dataSize * sizeof(float)) / 1'000'000 << " MB) for " << distName << std::endl;

    std::vector<float> data{generateRandomData(distribution, seed, dataSize)};

    if (sorted) {
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Sorting vector..." << std::endl;
        std::sort(data.begin(), data.end());
    }

    return data;
}

void testCompressionDecompression(std::ofstream& file, const std::vector<float>& data, const std::string& dataName, const int& seed, const int& trials, const int& level) {
    // Collect original distribution stats
    CompressionDecompressionResults results{};
    TH1F* h{vectorToHistogram(data, dataName)};
    results.originalSize = data.size();
    results.originalMean = h->GetMean();
    results.originalMeanError = h->GetMeanError();
    results.originalRMS = h->GetRMS();
    results.originalRMSError = h->GetRMSError();
    delete h;

    // Test compression and decompression for each number of bits
    for (int bits{0}; bits <= 23; bits++) {
        for (int trial{1}; trial <= trials; trial++) {
            // Compress
            std::cout << "(" << std::chrono::system_clock::now() << ") ";
            std::cout << "Compressing with at level " << level << " with " << bits << "-bit truncation" << std::endl;

            CompressionResults compResult{timedCompress(data, bits, level)};
            results.originalSize = data.size();
            results.compressedSize = compResult.compressedData.size();
            results.compressionDuration = compResult.duration;

            // Decompress
            std::cout << "(" << std::chrono::system_clock::now() << ") ";
            std::cout << "Decompressing..." << std::endl;

            DecompressionResults decompResult{timedDecompress(compResult.compressedData, results.originalSize)};
            results.decompressionDuration = decompResult.duration;

            // Collect decompressed distribution stats
            TH1F* h{vectorToHistogram(decompResult.decompressedData, std::format("hist_{}_{}", dataName, bits))};

            results.decompressedMean = h->GetMean();
            results.decompressedMeanError = h->GetMeanError();
            results.decompressedRMS = h->GetRMS();
            results.decompressedRMSError = h->GetRMSError();
            
            results.meanCE = std::abs(results.originalMean - results.decompressedMean);
            results.meanErrorCE = std::abs(results.originalMeanError - results.decompressedMeanError);
            results.rmsCE = std::abs(results.originalRMS - results.decompressedRMS);
            results.rmsErrorCE = std::abs(results.originalRMSError - results.decompressedRMSError);
            
            delete h;

            // Calculate compression ratio
            double compressionRatio{static_cast<double>(results.originalSize) / results.compressedSize};

            // Write results to file
            writeResults(file, results, dataName, bits, level, trial);
        }
    }
}

int main(int argc, char* argv[]) {
    // Process args
    // argv[1] = Distribution
    // argv[2] = Compression level
    int MODE{0};
    int LEVEL{0};
    int TRIALS{0};

    if (argc == 4) {
        MODE = std::stoi(argv[1]);
        LEVEL = std::stoi(argv[2]);
        TRIALS = std::stoi(argv[3]);
    }
    else {
        // Usage
        std::cout << "Usage: " << argv[0] << " <mode> <level> <numTrials>" << std::endl;
        std::cout << "Modes:" << std::endl;
        std::cout << "1: Uniform Random Data" << std::endl;    
        std::cout << "2: Normal Random Data" << std::endl;
        std::cout << "3: Sorted Uniform Random Data" << std::endl;
        std::cout << "4: Sorted Normal Random Data" << std::endl;
        std::cout << "Compression level: 0 - 9" << std::endl;
        return 1;
    }

    int dataSize{DATA_SIZE};
    int seed{12345};

    // Uniform distribution
    float min{0};
    float max{static_cast<float>(1 << 5)};
    std::string uniformName{std::format("uniform_min={}_max={}", min, max)};
    std::string uniformSortedName{std::format("uniform_sorted_min={}_max={}", min, max)};
    std::uniform_real_distribution<float> uniformDistribution(min, max);

    // Normal distribution
    float muNorm{16};
    float sigmaNorm{1.31};
    std::string normalName{std::format("normal_mu={}_sigma={}", muNorm, sigmaNorm)};
    std::string normalSortedName{std::format("normal_sorted_mu={}_sigma={}", muNorm, sigmaNorm)};
    std::normal_distribution<float> normalDistribution(muNorm, sigmaNorm);

    // Open file for writing
    std::string filename{std::format("test-zlib-d={}-l={}.csv", MODE, LEVEL)};
    std::ofstream file{filename};
    writeHeader(file);

    // Test distributions
    std::vector<float> testData{};
    switch(MODE) {
        case 1:
            testData = generateRandomTestData(uniformDistribution, uniformName, seed, dataSize, false);
            testCompressionDecompression(file, testData, uniformName, dataSize, TRIALS, LEVEL);
            break;
        case 2:
            testData = generateRandomTestData(normalDistribution, normalName, seed, dataSize, false);
            testCompressionDecompression(file, testData, normalName, dataSize, TRIALS, LEVEL);
            break;
        case 3:
            testData = generateRandomTestData(uniformDistribution, uniformSortedName, seed, dataSize, true);
            testCompressionDecompression(file, testData, uniformSortedName, dataSize, TRIALS, LEVEL);
            break;
        case 4:
            testData = generateRandomTestData(normalDistribution, normalSortedName, seed, dataSize, true);
            testCompressionDecompression(file, testData, normalSortedName, dataSize, TRIALS, LEVEL);
            break;
        default:
            std::cout << "Invalid distribution" << std::endl;
            return 1;
    }

    // Close file
    file.close();
        
    return 0;
}