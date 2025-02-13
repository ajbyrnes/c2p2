#include <chrono>
#include <iostream>

#include "compression_lib.hpp"

void compressionTest(const std::string& filepath, const int& trial, const int& trialMax) {
    // Read file data
    std::cout << "(" << std::chrono::system_clock::now() << ") Reading file: " << filepath << std::endl;
    std::vector<uint8_t> data{readData(filepath)};

    // Compress data
    std::cout << "(" << std::chrono::system_clock::now() << ") Compressing data..." << std::endl;
    std::chrono::high_resolution_clock::time_point compressionStart{std::chrono::high_resolution_clock::now()};
    std::vector<uint8_t> compressedData{zlibCompress(data)};
    std::chrono::high_resolution_clock::time_point compressionEnd{std::chrono::high_resolution_clock::now()};

    // Decompress data
    std::cout << "(" << std::chrono::system_clock::now() << ") Decompressing data..." << std::endl;
    std::chrono::high_resolution_clock::time_point decompressionStart{std::chrono::high_resolution_clock::now()};
    std::vector<uint8_t> decompressedData{zlibDecompress<uint8_t>(compressedData, data.size())};
    std::chrono::high_resolution_clock::time_point decompressionEnd{std::chrono::high_resolution_clock::now()};

    std::cout << std::endl;

    // Calculate results
    double compressionRatio{static_cast<double>(data.size()) / compressedData.size()};
    int64_t compressionTime{std::chrono::duration_cast<std::chrono::milliseconds>(compressionEnd - compressionStart).count()};
    int64_t decompressionTime{std::chrono::duration_cast<std::chrono::milliseconds>(decompressionEnd - decompressionStart).count()};

    // Print results
    std::cout << "File: " << filepath << std::endl;
    if (trialMax > 1) std::cout << "Trial: " << trial << "/" << trialMax << std::endl;
    std::cout << "Original data size: " << data.size() << " bytes" << std::endl;
    std::cout << "Compressed data size: " << compressedData.size() << " bytes" << std::endl;
    std::cout << "Compression ratio: " << compressionRatio << std::endl;

    std::cout << "Compression time: " << compressionTime << " ms" << std::endl;
    std::cout << "Decompression time: " << decompressionTime << " ms" << std::endl;
    std::cout << "Total time: " << compressionTime + decompressionTime << " ms" << std::endl;

    std::cout << std::endl;
}

int main(int argc, char* argv[]) { 
    // Read args
    int trialMax{1};

    if (argc > 1) {
        trialMax = std::stoi(argv[1]);
    }

    // Run tests
    for (const std::string& filepath : corpus) {
        for (int trial{1}; trial <= trialMax; ++trial) {
            compressionTest(filepath, trial, trialMax);
        }
    }

    return 0;
}