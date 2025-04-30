#ifndef MY_SZ_COMPRESSOR_HPP
#define MY_SZ_COMPRESSOR_HPP

#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <SZ3/api/sz.hpp>

#include "MyCompressor.hpp"

class SZCompressor : public MyCompressor {
    public:
        SZCompressor() {}
        
        SZCompressor(const int precision, const int errorBoundMode,
                        const int algo, const int interpAlgo, bool debug=false) : _debug(debug)
        {
            // Validate precision
            if (precision <= 0 || precision > 7) {
                throw std::invalid_argument("float precision must be between 1 and 7");
            }
            else {
                _precision = precision;
            }

            // Validate errorBoundMode
            // Values are EB_ABS, EB_REL, EB_PSNR, EB_L2NORM, EB_ABS_AND_REL, EB_ABS_OR_REL
            if (errorBoundMode < 0 || errorBoundMode > 5) {
                throw std::invalid_argument("errorBoundMode must be between 0 and 5");
            }
            else {
                _errorBoundMode = errorBoundMode;
            }

            // Validate algo
            // Values are ALGO_LORENZO_REG, ALGO_INTERP_LORENZO, ALGO_INTERP, ALGO_NOPRED, ALGO_LOSSLESS
            if (algo < 0 || algo > 4) {
                throw std::invalid_argument("algo must be between 0 and 4");
            }
            else {
                _algo = algo;
            }

            // Validate interpAlgo
            // Values are INTERP_ALGO_LINEAR, INTERP_ALGO_CUBIC
            if (interpAlgo < 0 || interpAlgo > 1) {
                throw std::invalid_argument("interpAlgo must be between 0 and 1");
            }
            else {
                _interpAlgo = interpAlgo;
            }
        }

        std::vector<uint8_t> compress(const std::vector<float>& data) override {
            if (_debug) {
                std::cerr << std::format("[DEBUG SZCompressor]: precision = {}, errorBoundMode = {}, algo = {}, interpAlgo = {}, dataSize = {}",
                                             _precision, _errorBoundMode, _algo, _interpAlgo, data.size() * sizeof(float)) << std::endl;
            }

            // Perform configuration
            std::vector<size_t> dims{data.size()};

            SZ3::Config conf({dims[0]});
            conf.lossless = false;
            conf.dataType = SZ_FLOAT;

            conf.cmprAlgo = static_cast<SZ3::ALGO>(_algo);
            conf.interpAlgo = static_cast<SZ3::INTERP_ALGO>(_interpAlgo);
            conf.errorBoundMode = static_cast<SZ3::EB>(_errorBoundMode);
            
            switch (_errorBoundMode) {
                case SZ3::EB_REL:
                    conf.relErrorBound = _calculateRelativeError(_precision);
                    break;
                case SZ3::EB_ABS:
                    conf.absErrorBound = 0.001;
                    break;
                case SZ3::EB_ABS_AND_REL:
                    conf.absErrorBound = 0.001;
                    conf.relErrorBound = _calculateRelativeError(_precision);
                    break;
                case SZ3::EB_ABS_OR_REL:
                    conf.absErrorBound = 0.001;
                    conf.relErrorBound = _calculateRelativeError(_precision);
                    break;
                default:
                    throw std::invalid_argument("Invalid error bound mode");
                    break;
            };

            // Compress data
            size_t compressedSize;
            char* compressedDataPtr{};
            if (_debug) {
                std::chrono::high_resolution_clock::time_point startCompression = std::chrono::high_resolution_clock::now();
                compressedDataPtr = SZ_compress(conf, data.data(), compressedSize);
                std::chrono::high_resolution_clock::time_point endCompression = std::chrono::high_resolution_clock::now();
                std::cerr << std::format("[DEBUG SZCompressor]: compression time = {} ms",
                                            std::chrono::duration_cast<std::chrono::milliseconds>(endCompression - startCompression).count()) << std::endl;
            }
            else {
                compressedDataPtr = SZ_compress(conf, data.data(), compressedSize);
            }

            // Wrap result in vector, then free
            std::vector<uint8_t> compressedData(compressedDataPtr, compressedDataPtr + compressedSize);
            free(compressedDataPtr);

            // Return compressed data
            return compressedData;
        }

        std::vector<float> decompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) override {
            // Decompress
            SZ3::Config conf{};
            float* decompressedDataPtr = nullptr;
            if (_debug) {
                std::chrono::high_resolution_clock::time_point startDecompression = std::chrono::high_resolution_clock::now();
                SZ_decompress(conf, reinterpret_cast<const char*>(compressedData.data()), compressedData.size() * sizeof(float), decompressedDataPtr);
                std::chrono::high_resolution_clock::time_point endDecompression = std::chrono::high_resolution_clock::now();
                std::cerr << std::format("[DEBUG SZCompressor]: decompression time = {} ms",
                                            std::chrono::duration_cast<std::chrono::milliseconds>(endDecompression - startDecompression).count()) << std::endl;
            }
            else {
                SZ_decompress(conf, reinterpret_cast<const char*>(compressedData.data()), compressedData.size() * sizeof(float), decompressedDataPtr);
            }

            // Wrap result in vector, then free
            std::vector<float> decompressedVec(decompressedDataPtr, decompressedDataPtr + uncompressedSize);
            free(decompressedDataPtr);

            // Return decompressed data
            return decompressedVec;
        }

        // Getters
        int getPrecision() const { return _precision; }
        int getErrorBoundMode() const { return _errorBoundMode; }
        int getAlgo() const { return _algo; }
        int getInterpAlgo() const { return _interpAlgo; }

        std::string getErrorBoundModeString() { return SZ3::enum2Str(static_cast<SZ3::EB>(_errorBoundMode)); }
        std::string getAlgoString() { return SZ3::enum2Str(static_cast<SZ3::ALGO>(_algo)); }
        std::string getInterpAlgoString() { return SZ3::enum2Str(static_cast<SZ3::INTERP_ALGO>(_interpAlgo)); }

        std::string getErrorBound() {
            switch(_errorBoundMode) {
                case SZ3::EB_REL:
                    return std::string("relative error: {}", _calculateRelativeError(_precision));
                case SZ3::EB_ABS:
                    return std::string("absolute error: {}", 0.001);
                case SZ3::EB_ABS_AND_REL:
                    return std::string("absolute error: {}, relative error: {}", 0.001, _calculateRelativeError(_precision));
                case SZ3::EB_ABS_OR_REL:
                    return std::string("absolute error: {}, relative error: {}", 0.001, _calculateRelativeError(_precision));
                default:
                    throw std::invalid_argument("Invalid error bound mode");
                    break;
            }
        }

    private:
        int _precision;
        int _errorBoundMode;
        int _algo;
        int _interpAlgo;
        bool _debug;

        double _calculateRelativeError(int precision) {
            return 0.5 * std::pow(10, -precision);
        }
};

#endif