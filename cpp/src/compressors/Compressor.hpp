
/**
 * @file Compressor.hpp
 * @brief Abstract base class for data compressors.
 */
#pragma once

#include <map>
#include <string>
#include <vector>

#include <cstdint>

#include "../utils/compression.hpp"


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
