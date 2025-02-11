
#include <cstdint>
#include <iostream>
#include <vector>

#include <zlib.h>
#include <SZ3/api/sz.hpp>

// zlib compression functions -------------------------------------------------
// From https://gitlab.cern.ch/-/snippets/3301

std::vector<unsigned char> compressWithZlib(const std::vector<float>& input) {
    uLong inputBytes{input.size() * sizeof(float)};
    uLong outputSize{compressBound(inputBytes)};

    std::vector<unsigned char> output(outputSize);
    
    int result{compress2(output.data(), &outputSize, reinterpret_cast<const Bytef*>(input.data()), inputBytes, Z_BEST_COMPRESSION)};
    if (result != Z_OK) {
        throw std::runtime_error("Compression failed.");
    }

    output.resize(outputSize);
    return output;
}

std::vector<float> decompressWithZlib(const std::vector<unsigned char>& input, size_t inputSize) {
    std::vector<float> output(inputSize / sizeof(float));
    uLong outputSize{inputSize};

    int result{uncompress(reinterpret_cast<Bytef*>(output.data()), &outputSize, input.data(), input.size())};
    if (result != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }

    return output;
}

// SZ compression functions ---------------------------------------------------

// Truncation functions --------------------------------------------------------

int truncateInt(int x, int nBits) {
    if (nBits == 0) return x;

    int mask{~((0x1 << nBits) - 1)};

    // Check for rounding
    int truncatedBits{x & ~mask};
    int roundingBit{0x1 << (nBits - 1)};
    if (truncatedBits >= roundingBit) {
        x += roundingBit;
    }

    return x & mask;
}

float truncateFloat(float f, int nBits) {
    if (nBits == 0) return f;

    uint32_t mask{~((0x1 << nBits) - 1)};
    uint32_t intVal{*reinterpret_cast<uint32_t*>(&f)};

    // Check for rounding
    uint32_t truncatedBits{intVal & ~mask};
    uint32_t roundingBit{0x1 << (nBits - 1)};
    if (truncatedBits >= roundingBit) {
        intVal += roundingBit;
    }

    uint32_t maskedIntVal{intVal & mask};
    return *reinterpret_cast<float*>(&maskedIntVal);
}

std::vector<float> truncate(const std::vector<float>& input, int nBits) {
    std::vector<float> output;
    output.reserve(input.size());
    for (float f : input) {
        output.push_back(truncateFloat(f, nBits));
    }
    return output;
}