#pragma once

#include <vector>
#include <stdexcept>

#include <cstdint>
#include <cstring>

#include <zlib.h>

#include "Compressor.hpp"

class TruncCompressor : public Compressor {
public:
    explicit TruncCompressor(int compressionLevel = Z_BEST_COMPRESSION, int mantissaBits = 8);

    // Setters and getters
    void setMantissaBits(int mantissaBits);
    int getMantissaBits() const;
    void setCompressionLevel(int level);
    int getCompressionLevel() const;

    // Reporting
    std::map<std::string, std::string> getConfig() const override;
    std::string toString() const override;

    // Compress input data, returns compressed data
    CompressedData compress(const UncompressedData& data) override;

    // Decompress input data, returns decompressed data
    std::vector<float> decompress(const CompressedData& compressedData) override;

private:
    int mantissaBits_ = 8; // Number of mantissa bits to keep (0-23 for float)
    int compressionLevel_ = Z_BEST_COMPRESSION;

    // Truncate mantissa of a float to mantissaBits bits, with rounding
    static std::vector<float> truncate_mantissas(const std::vector<float>& values, int mantissaBits);
};