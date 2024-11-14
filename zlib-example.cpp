// https://gitlab.cern.ch/-/snippets/3301

#include <vector>
#include <iostream>
#include <zlib.h>

// Function to compress data using zlib
std::vector<unsigned char> compress_vector(const std::vector<int>& data) {
    // Original data size
    uLong sourceSize = data.size() * sizeof(int);

    // Calculate buffer size for compressed data
    uLong destSize = compressBound(sourceSize);

    // Vector to hold compressed data
    std::vector<unsigned char> compressedData(destSize);

    // Compress the data
    int result{compress2(compressedData.data(), &destSize, reinterpret_cast<const Bytef*>(data.data()), sourceSize, Z_BEST_COMPRESSION)};
    
    if (result != Z_OK) {
        throw std::runtime_error("Compression failed.");
    }

    compressedData.resize(destSize);
    return compressedData;
}

std::vector<int> decompress_vector(const std::vector<unsigned char>& compressedData, size_t originalSize) {
    // Vector to hold decompressed data
    std::vector<int> decompressedData(originalSize / sizeof(int));
    uLong decompressedSize = originalSize;

    // Decompress the data
    int result{uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), &decompressedSize, compressedData.data(), compressedData.size())};

    if (result != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }

    return decompressedData;
}


int main() {
    // Original vector of integers
    std::vector<int> originalData{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Compress the vector
    std::vector<unsigned char> compressedData{compress_vector(originalData)};

    // Compare sizes
    std::cout << "Original size: " << originalData.size() * sizeof(int) << " bytes" << std::endl;
    std::cout << "Compressed size: " << compressedData.size() << " bytes" << std::endl;

    // Decompress vector
    std::vector<int> decompressedData{decompress_vector(compressedData, originalData.size() * sizeof(int))};

    // Verify the decompressed data
    if (originalData == decompressedData) {
        std::cout << "Decompression successful, data matches!" << std::endl;
    }
    else {
        std::cout << "Decompression failed, data does not match." << std::endl;
    }
}