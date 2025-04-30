#ifndef MY_TRUNK_COMPRESSOR_HPP
#define MY_TRUNK_COMPRESSOR_HPP

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <format>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <zlib.h>

#include "MyCompressor.hpp"

class TrunkCompressor : public MyCompressor {
    public:
        TrunkCompressor() {}

        TrunkCompressor(const int precision, const int compressionLevel, bool debug=false)
            : _debug(debug) 
        {
            // Calculate bits to truncate based on precision
            if (precision <= 0 || precision > 7) {
                throw std::invalid_argument("float precision must be between 1 and 7");
            }
            else {
                _precision = precision;
                _calculateBitsToTruncate();
            }
            

            // Set compression level
            if (compressionLevel < 0 || compressionLevel > 9) {
                throw std::invalid_argument("compressionLevel must be between 0 and 9");
            }
            else {
                _compressionLevel = compressionLevel;
            }
        }

        std::vector<uint8_t> compress(const std::vector<float>& data) override {
            if (_debug) {
                std::cerr << std::format("[DEBUG TrunkCompressor]: precision = {}, bitsTruncated = {}, compressionLevel = {}, dataSize = {}",
                                            _precision, _bitsTruncated, _compressionLevel, data.size() * sizeof(float)) << std::endl;
            }

            // Truncate data if _bitsTruncated > 0
            std::vector<float> truncatedData;
            if (_debug) {
                std::chrono::high_resolution_clock::time_point startTruncation{std::chrono::high_resolution_clock::now()};
                truncatedData = _bitsTruncated ? _truncateVector(data) : data;
                std::chrono::high_resolution_clock::time_point endTruncation{std::chrono::high_resolution_clock::now()};
                std::cerr << std::format("[DEBUG TrunkCompressor]: truncation time = {} ms",
                                            std::chrono::duration_cast<std::chrono::milliseconds>(endTruncation - startTruncation).count()) << std::endl;
            }
            else {
                truncatedData = _bitsTruncated ? _truncateVector(data) : data;
            }

            // Allocate vector for compressed data
            size_t compressedSize{compressBound(data.size() * sizeof(float))};
            std::vector<uint8_t> compressedData(compressedSize);

            // Compress
            int result;
            if (_debug) {
                std::chrono::high_resolution_clock::time_point startCompression{std::chrono::high_resolution_clock::now()};
                result = compress2(compressedData.data(), &compressedSize,
                            reinterpret_cast<const uint8_t*>(truncatedData.data()), 
                            truncatedData.size() * sizeof(float), _compressionLevel);
                std::chrono::high_resolution_clock::time_point endCompression{std::chrono::high_resolution_clock::now()};
                std::cerr << std::format("[DEBUG TrunkCompressor]: compression time = {} ms",
                                            std::chrono::duration_cast<std::chrono::milliseconds>(endCompression - startCompression).count()) << std::endl;
            }
            else {
                result = compress2(compressedData.data(), &compressedSize,
                            reinterpret_cast<const uint8_t*>(truncatedData.data()), 
                            truncatedData.size() * sizeof(float), _compressionLevel);
            }

            // Check for errors
            if (result != Z_OK) {
                throw std::runtime_error("TrunkCompressor: compression failed");
            }

            // Resize to actual size
            compressedData.resize(compressedSize);

            // Return compressed data
            return compressedData;
        }

        std::vector<float> decompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) override {
            // Allocate space for decompressed data
            std::vector<float> decompressedData(uncompressedSize);

            // Decompress
            size_t uncompressedSizeBytes{uncompressedSize * sizeof(float)};
            int result;
            if (_debug) {
                std::chrono::high_resolution_clock::time_point startDecompression{std::chrono::high_resolution_clock::now()};
                result = uncompress(reinterpret_cast<uint8_t*>(decompressedData.data()), &uncompressedSizeBytes,
                                                                compressedData.data(), compressedData.size());
                std::chrono::high_resolution_clock::time_point endDecompression{std::chrono::high_resolution_clock::now()};
                std::cerr << std::format("[DEBUG TrunkCompressor]: decompression time = {} ms",
                                            std::chrono::duration_cast<std::chrono::milliseconds>(endDecompression - startDecompression).count()) << std::endl;
            }
            else {
                result = uncompress(reinterpret_cast<uint8_t*>(decompressedData.data()), &uncompressedSizeBytes,
                                                                compressedData.data(), compressedData.size());
            }

            // Check for errors
            if (result != Z_OK) {
                throw std::runtime_error("TrunkCompressor: decompression failed");
            }

            // Return decompressed data
            return decompressedData;
        }

        // Getters
        int getPrecision() const { return _precision; }
        int getBitsTruncated() const { return _bitsTruncated; }

    private:
        int _precision;
        int _bitsTruncated;
        int _compressionLevel;
        bool _debug;

        void _calculateBitsToTruncate() {
            if (_precision == 7) {
                _bitsTruncated = 0;
            }
            else {
                _bitsTruncated = 23 - static_cast<int>(std::ceil(std::log2(std::pow(10, _precision))));
            }
        }

        float _truncateFloat(float value) {
            // Convert float to int
            uint32_t intVal{*reinterpret_cast<uint32_t*>(&value)};

            // Create masks for dropping and keeping bits
            uint32_t dropMask{((1u << _bitsTruncated) - 1u)};
            uint32_t keepMask{~dropMask};        // This is also the maximum truncated value

            // Truncate value
            uint32_t truncatedIntVal{intVal & keepMask};

            // Round-to-even if value won't overflow
            if (truncatedIntVal < keepMask) {
                // Round-to-even has us round up if the truncated bits are greater than 2^(bits - 1)
                // Ex: If bits = 4, we round up if the 4 dropped bits are greater than 2^3 = 1000
                uint32_t droppedVal{intVal & dropMask};      // The dropped bits
                uint32_t roundUpLimit{(1u << (_bitsTruncated - 1u))};

                if (droppedVal > roundUpLimit) {
                    truncatedIntVal = ((truncatedIntVal >> _bitsTruncated) + 1u) << _bitsTruncated;
                }
            }

            // Return truncated value as float
            return *reinterpret_cast<float*>(&truncatedIntVal);
        }

        std::vector<float> _truncateVector(const std::vector<float>& data) {
            // Allocate truncated basket
            std::vector<float> truncatedData(data.size());

            // Truncate basket
            std::transform(data.begin(), data.end(), truncatedData.begin(), 
                        [this](float value) { return _truncateFloat(value); });

            return truncatedData;
        }
};


#endif