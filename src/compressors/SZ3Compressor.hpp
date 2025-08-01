
/**
 * @file SZ3Compressor.hpp
 * @brief SZ3Compressor class for lossy and lossless compression using SZ3 library.
 */

#include "Compressor.hpp"
#include <vector>
#include <memory>

/**
 * @class SZ3Compressor
 * @brief Compressor using the SZ3 library for scientific data.
 */
class SZ3Compressor : public Compressor {
public:
    /**
     * @brief Construct an SZ3Compressor.
     * @param algorithm SZ3 algorithm (default: 0).
     * @param errorBound Relative or absolute error bound (default: 1e-5).
     * @param absError If true, use absolute error; otherwise, use relative error.
     */
    SZ3Compressor(int algorithm = 0, double errorBound = 1e-5, bool absError = false);

    /**
     * @brief Compress input data.
     * @param data Uncompressed data to compress.
     * @return CompressedData containing compressed result.
     */
    CompressedData compress(const UncompressedData& data) override;

    /**
     * @brief Decompress input data.
     * @param compressed Compressed data to decompress.
     * @return DecompressedData containing decompressed result.
     */
    DecompressedData decompress(const CompressedData& compressed) override;

    /** Getters and setters for error bounds and algorithm. */
    double getRelError() const;
    void setRelError(double relError);

    double getAbsError() const;
    void setAbsError(double absError);

    int getAlgorithm() const;
    void setAlgorithm(int algorithm);

    /** Reporting. */
    std::map<std::string, std::string> getConfig() const override;
    std::string toString() const override;

private:
    double absError_;    ///< Absolute error bound
    double relError_;    ///< Relative error bound
    int algorithm_;      ///< SZ3 algorithm selection
};