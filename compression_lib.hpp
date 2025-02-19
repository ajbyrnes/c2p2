#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include <zlib.h>

std::vector<std::string> corpus{
    "data/artificl.tar",        // Pathological behavior
    "data/calgary.tar",         // English text, Geophysical data, C/LISP/PASCAL, Terminal session
    "data/cantrbry.tar",        // English text, C/HTML, Excel spreadsheet, SPARC executable
    "data/large.tar",           // E. coli genome, King James bible, CIA world fact book
    "data/misc.tar"             // First 1,000,000 digits of pi    
};    

// Read/write functions --------------------------------------------------------------------------------------------
std::vector<uint8_t> readData(const std::string& filename) {
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

void writeData(const std::string& filename, const std::vector<uint8_t>& data) {
    // Open file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("writeData: failed to open file " + filename);
    }

    // Write data to file
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// zlib compression ----------------------------------------------------------------------------------------------
// Modified from https://gitlab.cern.ch/-/snippets/3301

template <typename T>
std::vector<uint8_t> zlibCompress(const std::vector<T>& data, const int& level=Z_BEST_COMPRESSION) {
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
    int32_t intVal{*reinterpret_cast<int32_t*>(&value)};

    // Create masks for dropping and keeping bits
    int32_t dropMask{((static_cast<int32_t>(1) << bits) - static_cast<int32_t>(1))};
    int32_t keepMask{~dropMask};        // This is also the maximum truncated value

    // Truncate value
    int32_t truncatedIntVal{intVal & keepMask};

    // Round-to-even if value won't overflow
    if (round && truncatedIntVal < keepMask) {
        // Round-to-even has us round up if the truncated bits are greater than 2^(bits - 1)
        // Ex: If bits = 4, we round up if the 4 dropped bits are greater than 2^3 = 1000
        int32_t droppedVal{intVal & dropMask};      // The dropped bits
        int32_t roundUpLimit{(1 << (bits - 1))};

        if (droppedVal > roundUpLimit) {
            truncatedIntVal = ((truncatedIntVal >> bits) + 1) << bits;
        }
    }
    
    // Return truncated value as float
    return *reinterpret_cast<float*>(&truncatedIntVal);
}

// SZ3 compression ------------------------------------------------------------------------------------------------