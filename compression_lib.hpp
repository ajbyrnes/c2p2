#include <bitset>
#include <cfloat>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "TH1F.h"

#include <zlib.h>
#include <SZ3/api/sz.hpp>

// #include "SZ3/utils/Config.hpp"

std::vector<std::string> corpus{
    "data/artificl.tar",        // Pathological behavior
    "data/calgary.tar",         // English text, Geophysical data, C/LISP/PASCAL, Terminal session
    "data/cantrbry.tar",        // English text, C/HTML, Excel spreadsheet, SPARC executable
    "data/large.tar",           // E. coli genome, King James bible, CIA world fact book
    "data/misc.tar"             // First 1,000,000 digits of pi    
};    

constexpr int DATA_SIZE{400'000'000 / sizeof(float)};

// zlib compression ----------------------------------------------------------------------------------------------
// Modified from https://gitlab.cern.ch/-/snippets/3301

template <typename T>
std::vector<uint8_t> zlibCompress(const std::vector<T>& data, const int& level=Z_DEFAULT_COMPRESSION) {
    // Calculate length of uncompressed data
    uLong sourceLen{data.size() * sizeof(T)};

    // Calculate maximum length of compressed data
    uLong destLen{compressBound(sourceLen)};

    // Allocate memory for compressed data
    std::vector<uint8_t> compressedData(destLen);

    // Compress data
    int result{compress2(compressedData.data(), &destLen, reinterpret_cast<const Bytef*>(data.data()), sourceLen, level)};
    if (result != Z_OK) {
        throw std::runtime_error("zlibCompress: compress2 failed with error " + std::to_string(result));
    }

    // Resize memory allocated to compressed data to its actual size
    compressedData.resize(destLen);

    return compressedData;
}

template <typename T>
std::vector<T> zlibDecompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) {
    // Allocate memory for decompressed data
    std::vector<T> decompressedData(uncompressedSize);

    // Decompress data
    uLongf destLen{uncompressedSize * sizeof(T)};
    int result{uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &destLen, compressedData.data(), compressedData.size())};
    if (result != Z_OK) {
        throw std::runtime_error("zlibDecompress: uncompress failed with error " + std::to_string(result));
    }

    return decompressedData;
}

// Bit truncation -------------------------------------------------------------------------------------------------

float truncate(float value, const int& bits, bool round=true) {
    // Do nothing for 0 bits
    if (bits == 0) return value;

    // Only mantissa bits should be truncated
    if (bits < 0 || bits > 23) {
        throw std::runtime_error("truncateNoRounding: bits must be between 0 and 23");
    }

    // Convert float to int
    uint32_t intVal{*reinterpret_cast<uint32_t*>(&value)};

    // Create masks for dropping and keeping bits
    uint32_t dropMask{((1u << bits) - 1u)};
    uint32_t keepMask{~dropMask};        // This is also the maximum truncated value

    // Truncate value
    uint32_t truncatedIntVal{intVal & keepMask};

    // Round-to-even if value won't overflow
    if (round && truncatedIntVal < keepMask) {
        // Round-to-even has us round up if the truncated bits are greater than 2^(bits - 1)
        // Ex: If bits = 4, we round up if the 4 dropped bits are greater than 2^3 = 1000
        uint32_t droppedVal{intVal & dropMask};      // The dropped bits
        uint32_t roundUpLimit{(1u << (bits - 1u))};

        if (droppedVal > roundUpLimit) {
            truncatedIntVal = ((truncatedIntVal >> bits) + 1u) << bits;
        }
    }
    
    // Return truncated value as float
    return *reinterpret_cast<float*>(&truncatedIntVal);
}

std::vector<uint8_t> zlibTruncateCompress(const std::vector<float>& data, const int& bits) {
    // Truncate data
    std::vector<float> truncatedData(data.size());
    for (size_t i{0}; i < data.size(); i++) {
        truncatedData[i] = truncate(data[i], bits);
    }

    // Compress with zlib
    return zlibCompress(truncatedData);
}

// SZ3 compression ------------------------------------------------------------------------------------------------
// Modified from https://github.com/physnerds/ldrd_compressions/blob/feature/tree/tests/mytest.cxx

std::vector<uint8_t> szCompress(std::vector<float> data, 
                                SZ3::EB errorBoundMode=SZ3::EB_ABS,
                                SZ3::ALGO algo=SZ3::ALGO_INTERP_LORENZO,
                                SZ3::INTERP_ALGO ialgo=SZ3::INTERP_ALGO_LINEAR,
                                float absErrorBound=1E-3) {

    std::cout << "Compressing data..." << std::endl;
    // Configuration
    SZ3::Config conf({data.size() * sizeof(float)});
    conf.errorBoundMode = errorBoundMode;
    conf.cmprAlgo = algo;
    conf.interpAlgo = ialgo;
    conf.absErrorBound = absErrorBound;

    // Compress
    size_t compressedSize;
    char* compressedData{SZ_compress(conf, data.data(), compressedSize)};

    // Return compressed data
    return std::vector<uint8_t>(compressedData, compressedData + compressedSize);
}

std::vector<float> szDecompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) {
    std::cout << "Decompressing data..." << std::endl;
    // Configuration
    SZ3::Config conf({uncompressedSize});

    // Convert compressed data to char*
    const char* compressedDataPtr{reinterpret_cast<const char*>(compressedData.data())};
    
    // Allocate memory for decompressed data
    float* decompressedDataPtr{};

    SZ_decompress(conf, compressedDataPtr, compressedData.size(), decompressedDataPtr);

    return std::vector<float>(decompressedDataPtr, decompressedDataPtr + uncompressedSize);
}

// Read/write functions --------------------------------------------------------------------------------------------

std::vector<uint8_t> readChars(const std::string& filename) {
    // Open file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("readData: failed to open file " + filename);
    }

    // Read file into vector
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Return data
    return data;
}

void writeChars(const std::string& filename, const std::vector<uint8_t>& data) {
    // Open file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("writeData: failed to open file " + filename);
    }

    // Write data to file
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// Data generation -------------------------------------------------------------------------------------------------

template <typename Distribution>
std::vector<float> generateRandomData(Distribution distribution, int dataSize=DATA_SIZE) {
    // Allocate basket
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
    // Allocate basket
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

// Other utilities -------------------------------------------------------------------------------------------------

TH1F* vectorToHistogram(const std::vector<float>& data, const std::string& name) {
    // Get min, max values
    float basketMin{*std::min_element(data.begin(), data.end())};
    float basketMax{*std::max_element(data.begin(), data.end())};

    // Create histogram
    TH1F* h{new TH1F(name.c_str(), name.c_str(), data.size(), basketMin, basketMax)};

    // Fill histogram
    for (const float& value : data) {
        h->Fill(value);
    }

    return h;
}

std::vector<float> truncateVectorData(const std::vector<float>& data, const int& bits) {
    // Allocate truncated basket
    std::vector<float> truncatedBasket(data.size());

    // Truncate basket
    for (size_t i{0}; i < data.size(); i++) {
        truncatedBasket[i] = truncate(data[i], bits);
    }

    return truncatedBasket;
}

