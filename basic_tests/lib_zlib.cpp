#include <algorithm>
#include <string>
#include <stdexcept>

#include "lib_utils.hpp"
#include "lib_zlib.hpp"

// Test functions

CompressionResults timedZlibCompress(std::vector<char> data, const int& level=Z_DEFAULT_COMPRESSION) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{zlibCompress<char>(data, level)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {compressedData, duration};
}

CompressionResults timedZlibTruncateCompress(std::vector<float> data, const int& bits, const int& level) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{zlibTruncateCompress(data, bits, level)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {compressedData, duration};      // This is needlessly pythonic, should be using pass-by-ref instead
}

// Compression -----------------------------------------------------------------------------------------------------

std::vector<uint8_t> zlibTruncateCompress(const std::vector<float>& data, const int& bits, const int& level) {
    // Truncate data
    std::vector<float> truncatedData(data.size());
    std::transform(data.begin(), data.end(), truncatedData.begin(), 
                   [bits](float value) { return truncate(value, bits); });

    // Compress with zlib
    return zlibCompress(truncatedData, level);
}

// Bit truncation -------------------------------------------------------------------------------------------------

float truncate(float value, const int& bits, const bool& round) {
    // Do nothing for 0 bits
    if (bits == 0) return value;

    // Only mantissa bits should be truncated
    if (bits < 0 || bits > 23) {
        throw std::runtime_error("truncateNoRounding: bits must be between 0 and 23");
    }

    // Convert float to int
    uint32_t intVal{*reinterpret_cast<uint32_t*>(&value)};

    // Create masks for dropping and keeping bits
    uint32_t dropMask{((1u << bits) - 1u)};
    uint32_t keepMask{~dropMask};        // This is also the maximum truncated value

    // Truncate value
    uint32_t truncatedIntVal{intVal & keepMask};

    // Round-to-even if value won't overflow
    if (round && truncatedIntVal < keepMask) {
        // Round-to-even has us round up if the truncated bits are greater than 2^(bits - 1)
        // Ex: If bits = 4, we round up if the 4 dropped bits are greater than 2^3 = 1000
        uint32_t droppedVal{intVal & dropMask};      // The dropped bits
        uint32_t roundUpLimit{(1u << (bits - 1u))};

        if (droppedVal > roundUpLimit) {
            truncatedIntVal = ((truncatedIntVal >> bits) + 1u) << bits;
        }
    }
    
    // Return truncated value as float
    return *reinterpret_cast<float*>(&truncatedIntVal);
}

std::vector<float> truncateVectorData(const std::vector<float>& data, const int& bits) {
    // Allocate truncated basket
    std::vector<float> truncatedBasket(data.size());

    // Truncate basket
    std::transform(data.begin(), data.end(), truncatedBasket.begin(), 
                   [bits](float value) { return truncate(value, bits); });

    return truncatedBasket;
}

