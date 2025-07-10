#include "SZ3Compressor.hpp"
#include <SZ3/api/sz.hpp>

SZ3Compressor::SZ3Compressor(double relError, int algorithm)
    : relError_(relError), algorithm_(algorithm) {}

CompressedData SZ3Compressor::compress(const UncompressedData& data) {
    // Config
    std::vector<size_t> dims{data.data.size()};
    SZ3::Config config({dims[0]});

    config.lossless = false;
    config.dataType = SZ_FLOAT;
    config.errorBoundMode = SZ3::EB_REL; // Relative error bound
    config.relErrorBound = relError_;
    config.cmprAlgo = static_cast<SZ3::ALGO>(algorithm_);

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

int SZ3Compressor::getAlgorithm() const { return algorithm_; }
void SZ3Compressor::setAlgorithm(int algorithm) { algorithm_ = algorithm; }

std::map<std::string, std::string> SZ3Compressor::getConfig() const {
    return {
        {"relError", std::format("{:.10f}", relError_)},
        {"algorithm", std::to_string(algorithm_)}
    };
}

std::string SZ3Compressor::toString() const {
    return "SZ3Compressor{" + std::format("rel={:.10f};algo={}", relError_, algorithm_) + "}";
}