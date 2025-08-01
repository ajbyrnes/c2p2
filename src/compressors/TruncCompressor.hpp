
/**
 * @file TruncCompressor.hpp
 * @brief TruncCompressor class for lossy float compression using mantissa truncation and zlib.
 */

#include <vector>
#include <stdexcept>
#include <cstdint>
#include <cstring>
#include <zlib.h>
#include "Compressor.hpp"

/**
 * @class TruncCompressor
 * @brief Compressor that truncates mantissa bits of floats and compresses with zlib.
 */
class TruncCompressor : public Compressor {
public:
    /**
     * @brief Construct a TruncCompressor.
     * @param compressionLevel zlib compression level (default: Z_BEST_COMPRESSION).
     * @param mantissaBits Number of mantissa bits to keep (default: 8).
     */
    explicit TruncCompressor(int compressionLevel = Z_BEST_COMPRESSION, int mantissaBits = 8);

    /** Setters and getters for mantissa bits and compression level. */
    void setMantissaBits(int mantissaBits);
    int getMantissaBits() const;
    void setCompressionLevel(int level);
    int getCompressionLevel() const;

    /** Reporting. */
    std::map<std::string, std::string> getConfig() const override;
    std::string toString() const override;

    /**
     * @brief Compress input data.
     * @param data Uncompressed data to compress.
     * @return CompressedData containing compressed result.
     */
    CompressedData compress(const UncompressedData& data) override;

    /**
     * @brief Decompress input data.
     * @param compressedData Compressed data to decompress.
     * @return DecompressedData containing decompressed result.
     */
    DecompressedData decompress(const CompressedData& compressedData) override;

private:
    int mantissaBits_ = 8; ///< Number of mantissa bits to keep (0-23 for float)
    int compressionLevel_ = Z_BEST_COMPRESSION; ///< zlib compression level

    /**
     * @brief Truncate mantissa of floats to mantissaBits bits, with rounding.
     * @param values Vector of float values.
     * @param mantissaBits Number of mantissa bits to keep.
     * @return Vector of truncated float values.
     */
    static std::vector<float> truncate_mantissas(const std::vector<float>& values, int mantissaBits);
};