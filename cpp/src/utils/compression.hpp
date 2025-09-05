
/**
 * @file compression.hpp
 * @brief Data structures for representing uncompressed, compressed, and decompressed data.
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * @struct UncompressedData
 * @brief Structure to hold uncompressed floating-point data and metadata.
 *
 * Members:
 *   data      Uncompressed float data
 *   dataName  Name of the data
 *   fileName  Name of the file containing the data
 *   dims      Dimensions of the data
 *   numFloats Number of floats in the data
 */
struct UncompressedData {
    std::vector<float> data{}; // Uncompressed data
    std::string dataName{};
    std::string fileName{}; // Name of the file containing the data
    std::vector<size_t> dims{};
    size_t numFloats = 0;
};

/**
 * @struct CompressedData
 * @brief Structure to hold compressed data and related metadata.
 *
 * Members:
 *   data           Compressed data (bytes)
 *   numFloats      Number of floats in the original data
 *   dataName       Name of the data
 *   ogDataFileName Name of the original data file
 *   dims           Dimensions of the data
 */
struct CompressedData {
    std::vector<uint8_t> data; // Compressed data
    size_t numFloats;          // Number of floats in the original data
    std::string dataName;     // Name of the data
    std::string ogDataFileName; // Name of the original data file (if applicable
    std::vector<size_t> dims{};
};

/**
 * @struct DecompressedData
 * @brief Structure to hold decompressed data and metadata after decompression.
 *
 * Members:
 *   data           Decompressed float data
 *   compressor     Name of the compressor used
 *   dataName       Name of the data
 *   ogDataFileName Name of the original data file
 */
struct DecompressedData {
    std::vector<float> data; // Decompressed data
    std::string compressor;  // Name of the compressor used
    std::string dataName;
    std::string ogDataFileName;    // Name of the file containing the decompressed data
};