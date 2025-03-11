#ifndef TEST_LIB_HPP
#define TEST_LIB_HPP

#include <chrono>
#include <fstream>
#include <random>
#include <vector>

#include <zlib.h>
#include <TH1F.h>

#include "lib_zlib.hpp"

const std::vector<std::string> CORPUS{
    "data/artificl.tar",        // Pathological behavior
    "data/calgary.tar",         // English text, Geophysical data, C/LISP/PASCAL, Terminal session
    "data/cantrbry.tar",        // English text, C/HTML, Excel spreadsheet, SPARC executable
    "data/large.tar",           // E. coli genome, King James bible, CIA world fact book
    "data/misc.tar"             // First 1,000,000 digits of pi    
};    

constexpr int MB{1'000'000};
constexpr int KB{1'000};
constexpr int DATA_SIZE{(250 * MB) / sizeof(float)};

struct CompressionResults {
    std::vector<uint8_t> compressedData{};
    std::chrono::duration<long, std::nano> duration{};
};

template <typename T>
struct DecompressionResults {
    std::vector<T> decompressedData{};
    std::chrono::duration<long, std::nano> duration{};
};

struct CompressionDecompressionResults {
    std::string dataName{};
    int compressionLevel{};
    int trial{};
    size_t originalSize{};
    float originalMean{};
    float originalMeanError{};
    float originalRMS{};
    float originalRMSError{};
    size_t compressedSize{};
    float compressionRatio{};
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

CompressionResults timedZlibCompress(std::vector<uint8_t> data, const int& level);
CompressionResults timedZlibTruncateCompress(std::vector<float> data, const int& bits, const int& level=Z_DEFAULT_COMPRESSION);

template<typename T>
DecompressionResults<T> timedZlibDecompress(std::vector<uint8_t> compressedData, const size_t& originalDataSize) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Decompress data
    std::vector<T> decompressedData{zlibDecompress<T>(compressedData, originalDataSize)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {decompressedData, duration};      
}

std::vector<uint8_t> charFileToVector(const std::string& filename);
void vectorToCharFile(const std::string& filename, const std::vector<uint8_t>& data);

template <typename Distribution>
std::vector<float> generateRandomData(Distribution distribution, int dataSize=DATA_SIZE) {
    // Allocate Data
    std::vector<float> data(dataSize);

    // Seed generator with random device
    std::random_device rd;
    std::mt19937 generator(rd());

    // Generate data
    for (int i{0}; i < dataSize; i++) {
        data[i] = distribution(generator);
    }

    return data;
}

template <typename Distribution>
std::vector<float> generateRandomData(Distribution distribution, int seed, int dataSize=DATA_SIZE) {
    // Allocate Data
    std::vector<float> data(dataSize);

    // Seed generator with provided seed
    std::mt19937 generator;
    generator.seed(seed);

    // Generate data
    for (int i{0}; i < dataSize; i++) {
        data[i] = distribution(generator);
    }

    return data;
}

TH1F* vectorToHistogram(const std::vector<float>& data, const std::string& name, const int& bins=100, const float& min=0, const float& max=1);

#endif