#include <format>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "TrunkCompressor.hpp"
#include "SZCompressor.hpp"

class CompressorBench{
    public:
        enum COMPRESSOR{TRUNK, SZ};

        CompressorBench(const int iterations, const std::string dataName, const int precision, 
                        const int trunkCompressionLevel,
                        const int szErrorBoundMode, const int szAlgo, const int szInterpAlgo,
                        const bool debug=false)
            :   _dataName(dataName), _precision(precision), _trunkCompressionLevel(trunkCompressionLevel),
                _szErrorBoundMode(szErrorBoundMode), _szAlgo(szAlgo), _szInterpAlgo(szInterpAlgo)
        {
            // Validation iterations
            if (iterations <= 0) {
                throw std::invalid_argument("Iterations must be greater than 0");
            }
            _iterations = iterations;

            // Create compressor objects
            _trunkCompressor = TrunkCompressor(precision, trunkCompressionLevel, debug);
            _szCompressor = SZCompressor(precision, szErrorBoundMode, szAlgo, szInterpAlgo, debug);
        }

        void run(const std::vector<float>& data, const int dataLength)
        {
            // Validate data size
            if (dataLength <= 0) {
                throw std::invalid_argument("Data size must be greater than 0");
            }
            _originalDataSize = dataLength * sizeof(float);
            
            for (int compressor{TRUNK}; compressor <= SZ; ++compressor) {
                std::vector<uint8_t> compressedData{};
                std::vector<float> decompressedData{};

                // Selection compression/decompression functions
                std::function<std::vector<uint8_t>(const std::vector<float>&)> compressFunc;
                std::function<std::vector<float>(const std::vector<uint8_t>&, const size_t&)> decompressFunc;
                switch (compressor) {
                    case TRUNK:
                        compressFunc = std::bind(&TrunkCompressor::compress, &_trunkCompressor, std::placeholders::_1);
                        decompressFunc = std::bind(&TrunkCompressor::decompress, &_trunkCompressor, std::placeholders::_1, std::placeholders::_2);
                        break;
                    case SZ:
                        compressFunc = std::bind(&SZCompressor::compress, &_szCompressor, std::placeholders::_1);
                        decompressFunc = std::bind(&SZCompressor::decompress, &_szCompressor, std::placeholders::_1, std::placeholders::_2);
                        break;
                    default:
                        throw std::invalid_argument("Invalid compressor type");
                };

                // Time compression
                for (int i = 0; i < _iterations; ++i) {
                    _start = std::chrono::high_resolution_clock::now();
                    compressedData.clear();
                    compressedData = compressFunc(data);
                    _end = std::chrono::high_resolution_clock::now();
                    _compressionTime[compressor] += std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
                }
                
                // Average over iterations
                _compressionTime[compressor] /= _iterations;

                // Record compressed data size
                _compressedDataSize[compressor] = compressedData.size() * sizeof(uint8_t);

                // Record compression ratio
                _compressionRatio[compressor] = static_cast<double>(_originalDataSize) / _compressedDataSize[compressor];

                // Time decompression
                for (int i = 0; i < _iterations; ++i) {
                    _start = std::chrono::high_resolution_clock::now();
                    decompressedData.clear();
                    decompressedData = decompressFunc(compressedData, dataLength);
                    _end = std::chrono::high_resolution_clock::now();
                    _decompressionTime[compressor] += std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
                }

                // Average over iterations
                _decompressionTime[compressor] /= _iterations;
            }
        }

        std::string generateReport() {
            std::string report{};

            for (int compressor{TRUNK}; compressor <= SZ; compressor++) {
                report += std::format("{},",(compressor == CompressorBench::TRUNK ? "Trunk" : "SZ"));
                report += std::format("{},", _iterations);
                report += std::format("{},", _dataName);
                report += std::format("{},", _precision);
                report += std::format("{},", (compressor == CompressorBench::TRUNK ? _trunkCompressionLevel : -1));
                report += std::format("{},", (compressor == CompressorBench::SZ ? _szErrorBoundMode : -1));
                report += std::format("{},", (compressor == CompressorBench::SZ ? _szAlgo : -1));
                report += std::format("{},", (compressor == CompressorBench::SZ ? _szInterpAlgo : -1));
                report += std::format("{},", _compressionTime[compressor]);
                report += std::format("{},", _decompressionTime[compressor]);
                report += std::format("{},", _compressionRatio[compressor]);
                report += std::format("{},", _compressedDataSize[compressor]);
                report += std::format("{}", _originalDataSize);
                report += "\n";
            }

            return report;
        }

        // Getters
        int getIterations() const {
            return _iterations;
        }
        
        double getCompressionTime(const COMPRESSOR compressor) const {
            return _compressionTime[compressor];
        }

        double getDecompressionTime(const COMPRESSOR compressor) const {
            return _decompressionTime[compressor];
        }

        double getCompressionRatio(const COMPRESSOR compressor) const {
            return _compressionRatio[compressor];
        }

        size_t getCompressedDataSize(const COMPRESSOR compressor) const {
            return _compressedDataSize[compressor];
        }

        size_t getOriginalDataSize() const {
            return _originalDataSize;
        }

        void reset() {  
            for (int compressor{TRUNK}; compressor <= SZ; ++compressor) {
                _compressionTime[compressor] = 0;
                _decompressionTime[compressor] = 0;
                _compressedDataSize[compressor] = 0;
                _compressionRatio[compressor] = 0;
            }
            _originalDataSize = 0;
        }

        

    private:
        TrunkCompressor _trunkCompressor;    
        SZCompressor _szCompressor;
        
        int _iterations;
        std::string _dataName;
        int _precision;

        int _trunkCompressionLevel;

        int _szErrorBoundMode;
        int _szAlgo;
        int _szInterpAlgo;

        size_t _originalDataSize;
        size_t _compressedDataSize[2];
        double _compressionTime[2];
        double _decompressionTime[2];
        double _compressionRatio[2];

        std::chrono::high_resolution_clock::time_point _start;
        std::chrono::high_resolution_clock::time_point _end;

};