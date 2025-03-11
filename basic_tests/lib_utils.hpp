#include <chrono>
#include <fstream>
#include <random>
#include <unistd.h>
#include <vector>

#include <TH1F.h>

#ifndef TEST_LIB_HPP
#define TEST_LIB_HPP

const std::vector<std::string> CORPUS{
    "data/artificl.tar",        // Pathological behavior
    "data/calgary.tar",         // English text, Geophysical data, C/LISP/PASCAL, Terminal session
    "data/cantrbry.tar",        // English text, C/HTML, Excel spreadsheet, SPARC executable
    "data/large.tar",           // E. coli genome, King James bible, CIA world fact book
    "data/misc.tar"             // First 1,000,000 digits of pi    
};    

constexpr int MB{1'000'000};
constexpr int KB{1'000};

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
    std::chrono::time_point<std::chrono::system_clock> timestamp{};
    std::string system{};
    std::string dataName{};
    int compressionLevel{};
    int bitsTruncated{};
    int errorBoundMode{};
    int algo{};
    int ialgo{};
    float absErrorBound{};
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

// File read/write ---------------------------------------------------------------------------------------------------

template<typename T>
std::vector<T> fileToVector(const std::string& filename) {
    // Open file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("readData: failed to open file " + filename);
    }

    // Read file into vector
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Return data
    return data;
}

template<typename T>
void vectorToFile(const std::string& filename, const std::vector<T>& data) {
    // Open file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("writeData: failed to open file " + filename);
    }

    // Write data to file
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// Data generation ---------------------------------------------------------------------------------------------------

template <typename Distribution>
std::vector<float> generateRandomData(Distribution distribution, int seed, int dataSize) {
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

std::string getHost();
TH1F* vectorToHistogram(const std::vector<float>& data, const std::string& name);

#endif