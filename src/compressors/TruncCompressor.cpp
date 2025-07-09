#include "TruncCompressor.hpp"
#include <format>
#include <stdexcept>
#include <zlib.h>
#include <cstring>
#include <algorithm>

TruncCompressor::TruncCompressor(int compressionLevel, int mantissaBits) {
    setCompressionLevel(compressionLevel);
    setMantissaBits(mantissaBits);
}

void TruncCompressor::setMantissaBits(int mantissaBits) {
    if (mantissaBits < 0 || mantissaBits > 23) {
        throw std::invalid_argument("mantissaBits must be in [0,23]");
    }
    mantissaBits_ = mantissaBits;
}

int TruncCompressor::getMantissaBits() const {
    return mantissaBits_;
}

void TruncCompressor::setCompressionLevel(int level) {
    if (level < 0 || level > 9) {
        throw std::invalid_argument("compressionLevel must be in [0,9]");
    }
    compressionLevel_ = level;
}

int TruncCompressor::getCompressionLevel() const {
    return compressionLevel_;
}

std::map<std::string, std::string> TruncCompressor::getConfig() const {
    return {
        {"mantissaBits", std::to_string(mantissaBits_)},
        {"compressionLevel", std::to_string(compressionLevel_)}
    };
}

std::string TruncCompressor::toString() const {
    return "TruncCompressor";
}

CompressedData TruncCompressor::compress(const UncompressedData& data) {
    std::vector<float> truncated{truncate_mantissas(data.data, mantissaBits_)};

    const uint8_t* input = reinterpret_cast<const uint8_t*>(truncated.data());
    uLong input_size = truncated.size() * sizeof(float);

    uLongf output_size{compressBound(input_size)};
    std::vector<uint8_t> output(output_size);

    int res{::compress2(output.data(), &output_size, input, input_size, Z_BEST_COMPRESSION)};
    if (res != Z_OK) {
        throw std::runtime_error("zlib compress2 failed");
    }

    output.resize(output_size);
    return CompressedData{output, data.data.size()};
}

std::vector<float> TruncCompressor::decompress(const CompressedData& compressedData) {
    std::vector<float> output(compressedData.numFloats);
    uLongf output_size{compressedData.numFloats * sizeof(float)};

    int res{::uncompress(reinterpret_cast<Bytef*>(output.data()), &output_size,
                        compressedData.data.data(), compressedData.data.size())};

    if (res != Z_OK) {
        throw std::runtime_error("zlib uncompress failed");
    }

    if (output_size != compressedData.numFloats * sizeof(float)) {
        throw std::runtime_error("Decompressed size mismatch");
    }
    return output;
}

std::vector<float> TruncCompressor::truncate_mantissas(const std::vector<float>& values, int mantissaBits) {
    if (mantissaBits < 0 || mantissaBits > 22) {
        return values; // No truncation needed
    }

    std::vector<float> result;
    result.reserve(values.size());
    for (const float& value : values) {
        union { float f; uint32_t u; } u{value};
        if (mantissaBits < 23) {
            uint32_t shift{23 - mantissaBits};
            uint32_t mask{~((1u << shift) - 1)};
            uint32_t round_bit{1u << (shift - 1)};
            // Add rounding bit before masking
            u.u += round_bit;
            u.u &= (0xFF800000 | mask); // keep sign, exponent, and top mantissaBits
        }
        result.push_back(u.f);
    }
    return result;
}