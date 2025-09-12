
/**
 * @file Compressor.hpp
 * @brief Abstract base class for data compressors.
 */
#pragma once

#include <map>
#include <string>
#include <vector>

#include <cstdint>

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

/**
 * @class Compressor
 * @brief Abstract base class for data compressors.
 *
 * Defines the interface for compression and decompression of data, as well as reporting configuration.
 */
class Compressor {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Compressor() = default;

    /**
     * @brief Compress input data.
     * @param data Uncompressed data to compress.
     * @return CompressedData containing compressed result.
     */
    virtual CompressedData compress(const UncompressedData& data) = 0;

    /**
     * @brief Decompress input data.
     * @param compressedData Compressed data to decompress.
     * @return DecompressedData containing decompressed result.
     */
    virtual DecompressedData decompress(const CompressedData& compressedData) = 0;

    /**
     * @brief Get configuration as a map of key-value pairs.
     * @return Map of configuration options.
     */
    virtual std::map<std::string, std::string> getConfig() const = 0;

    /**
     * @brief Get a string representation of the compressor.
     * @return String describing the compressor.
     */
    virtual std::string toString() const = 0;
};
