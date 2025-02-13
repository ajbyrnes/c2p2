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

float truncateNoRounding(float value, const int& bits) {
    // Do nothing for 0 bits
    if (bits == 0) return value;

    // Valid that only mantissa bits are truncated
    if (bits > 23) {
        throw std::runtime_error("truncateNoRounding: bits must be between 0 and 23");
    }

    // Create mask
    uint32_t mask{~((0x1 << bits) - 1u)};

    // Reinterpret float as int for truncation
    uint32_t intVal{*reinterpret_cast<uint32_t*>(&value)};

    // Truncate the value
    uint32_t truncatedIntVal{intVal & mask};

    // Return truncated value as float
    return *reinterpret_cast<float*>(&truncatedIntVal);
}

// SZ3 compression ------------------------------------------------------------------------------------------------