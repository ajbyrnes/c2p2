#include "SZ3Compressor.hpp"
#include <SZ3/api/sz.hpp>

SZ3::Config makeConfig(const UncompressedData& data) {
    std::vector<size_t> dims = data.dims;

    switch (data.dims.size()) {
        case 1:
            return SZ3::Config({dims[0]});
        case 2:
            return SZ3::Config({dims[0], dims[1]});
        case 3:
            return SZ3::Config({dims[0], dims[1], dims[2]});
        default:
            throw std::invalid_argument("Unsupported number of dimensions: " + std::to_string(data.dims.size()));
    }
}

SZ3Compressor::SZ3Compressor(int algorithm, double errorBound, bool absError) {
    // Set error bound
    if (absError) {
        relError_ = -1; // Set relative error to 0 if absolute error is used
        absError_ = errorBound;
    } else {
        relError_ = errorBound;
        absError_ = -1; // Set absolute error to 0 if relative error is used
    }

    // Set algorithm
    if (algorithm < 0 || algorithm > 3) {
        throw std::invalid_argument("Invalid algorithm specified. Must be between 0 and 3.");
    }

    algorithm_ = algorithm;
}

CompressedData SZ3Compressor::compress(const UncompressedData& data) {
    SZ3::Config config = makeConfig(data);
    
    config.lossless = false;
    config.dataType = SZ_FLOAT;
    config.cmprAlgo = static_cast<SZ3::ALGO>(algorithm_);

    if (relError_ == -1) {      // Use absolute error bound
        config.errorBoundMode = SZ3::EB_ABS;
        config.absErrorBound = absError_;
    }
    else {
        config.errorBoundMode = SZ3::EB_REL; // Use relative error bound
        config.relErrorBound = relError_;
    }

    // Compress
    size_t cmpSize = 0;
    char* cmpData{};

    cmpData = SZ_compress(
        config,
        data.data.data(),
        cmpSize
    );

    // std::cout << "[DEBUG] SZ_compress returned cmpData=" << static_cast<void*>(cmpData) << ", cmpSize=" << cmpSize << std::endl;
    if (!cmpData) {
        throw std::runtime_error("SZ_compress failed to allocate output buffer");
    }

    // std::cout << "[DEBUG] About to allocate compressedData vector of size: " << cmpSize << std::endl;
    // Move compressed data to CompressedData struct
    std::vector<uint8_t> compressedData(cmpData, cmpData + cmpSize);
    CompressedData compressed;
    compressed.data = compressedData;
    compressed.numFloats = data.data.size();

    // Free the compressed data pointer
    free(cmpData);

    return compressed;
}

std::vector<float> SZ3Compressor::decompress(const CompressedData& compressed) {
    // std::cout << "[DEBUG] Compressed data length: " << compressed.data.size() << std::endl;
    // std::cout << "[DEBUG] Number of floats to decompress: " << compressed.numFloats << std::endl;

    // Config
    std::vector<size_t> dims{compressed.numFloats};
    SZ3::Config config({dims[0]});

    config.lossless = false;
    config.dataType = SZ_FLOAT;
    config.errorBoundMode = SZ3::EB_REL; // Relative error bound
    config.relErrorBound = relError_;
    config.cmprAlgo = static_cast<SZ3::ALGO>(algorithm_);
    // Allocate output buffer
    float* dec_data_p = nullptr;

    // Call SZ_decompress as in the smoke test
    SZ_decompress(
        config,
        reinterpret_cast<const char*>(compressed.data.data()),
        compressed.numFloats,
        dec_data_p
    );
    // std::cout << "[DEBUG] SZ_decompress returned dec_data_p=" << static_cast<void*>(dec_data_p) << std::endl;
    if (!dec_data_p) {
        throw std::runtime_error("SZ_decompress failed to allocate output buffer");
    }
    // std::cout << "[DEBUG] About to allocate dec_data vector of size: " << compressed.numFloats << std::endl;
    std::vector<float> dec_data(dec_data_p, dec_data_p + compressed.numFloats);
    free(dec_data_p); // Free the decompressed data pointer
    
    return dec_data;
}


double SZ3Compressor::getRelError() const { return relError_; }
void SZ3Compressor::setRelError(double relError) { relError_ = relError; }

double SZ3Compressor::getAbsError() const { return absError_; }
void SZ3Compressor::setAbsError(double absError) { absError_ = absError; }

int SZ3Compressor::getAlgorithm() const { return algorithm_; }
void SZ3Compressor::setAlgorithm(int algorithm) { algorithm_ = algorithm; }

std::map<std::string, std::string> SZ3Compressor::getConfig() const {
    return {
        {"relError", std::format("{:.10f}", relError_)},
        {"absError", std::format("{:.10f}", absError_)},
        {"algorithm", std::to_string(algorithm_)}
    };
}

std::string SZ3Compressor::toString() const {
    return "SZ3Compressor{" + std::format("rel={:.10f};abs={:.10f};algo={}", relError_, absError_, algorithm_) + "}";
}