#ifndef COMPRESSOR_BENCH_HPP
#define COMPRESSOR_BENCH_HPP

#include <format>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "TrunkCompressor.hpp"
#include "SZCompressor.hpp"
#include "SZZlibCompressor.hpp"

struct BenchmarkParams {
    bool doTrunk;
    bool doSZ;
    bool sortData;

    int iterations;
    int precision;
    bool debug;

    double dataMB;
    std::string dataName;
    std::string sourceFile;
    std::string branchName;

    int seed;
    float mean;
    float stddev;

    int trunkCompressionLevel;

    int szErrorBoundMode;
    int szAlgo;
    int szInterpAlgo;
};

BenchmarkParams parseArguments(int argc, char* argv[]) {
    BenchmarkParams params;

    params.doTrunk = false;
    params.doSZ = true;
    params.sortData = false;

    params.iterations = 5;
    params.precision = 3;
    params.debug = false;

    params.dataMB = 0;
    params.dataName = "root";
    params.branchName = "lep_pt";
    params.sourceFile = "mc_361106.Zee.1largeRjet1lep.root";

    params.seed = 12345;
    params.mean = 0.0;
    params.stddev = 1.0f;

    params.trunkCompressionLevel = 9;
    params.szErrorBoundMode = SZ3::EB_REL;
    params.szAlgo = SZ3::ALGO_LORENZO_REG;
    params.szInterpAlgo = SZ3::INTERP_ALGO_LINEAR;

    // Read parameters
    for (int i{1}; i < argc; ++i) {
        std::string arg{argv[i]};
        if (arg == "--iterations") {
            params.iterations = std::stoi(argv[++i]);
        } else if (arg == "--precision") {
            params.precision = std::stoi(argv[++i]);
        } else if (arg == "--debug") {
            params.debug = std::stoi(argv[++i]);
        } else if (arg == "--dataMB") {
            params.dataMB = std::stod(argv[++i]);
        } else if (arg == "--dataSource") {
            params.dataName = argv[++i];
        } else if (arg == "--sourceFile") {
            params.sourceFile = argv[++i];
        } else if (arg == "--branchName") {
            params.branchName = argv[++i];
        } else if (arg == "--mean") {
            params.mean = std::stof(argv[++i]);
        } else if (arg == "--stddev") {
            params.stddev = std::stof(argv[++i]);
        } else if (arg == "--trunkCompressionLevel") {
            params.trunkCompressionLevel = std::stoi(argv[++i]);
        } else if (arg == "--szErrorBoundMode") {
            params.szErrorBoundMode = std::stoi(argv[++i]);
        } else if (arg == "--szAlgo") {
            params.szAlgo = std::stoi(argv[++i]);
        } else if (arg == "--szInterpAlgo") {
            params.szInterpAlgo = std::stoi(argv[++i]);
        } else if (arg == "--seed") {
            params.seed = std::stoi(argv[++i]);
        } else if (arg == "--doTrunk") {
            params.doTrunk = std::stoi(argv[++i]);
        } else if (arg == "--doSZ") {
            params.doSZ = std::stoi(argv[++i]);
        } else if (arg == "--sortData") {
            params.sortData = std::stoi(argv[++i]);
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }

    return params;
}

constexpr int NUMCOMPRESSORS{3};

class CompressorBench{
    public:
        enum COMPRESSOR{TRUNK, SZ};

        CompressorBench(const BenchmarkParams& params)
            :   _doSZ(params.doSZ), _doTrunk(params.doTrunk), _sortData(params.sortData),
                _dataName(params.dataName + "-" + params.branchName), _precision(params.precision),
                _trunkCompressionLevel(params.trunkCompressionLevel),
                _szErrorBoundMode(params.szErrorBoundMode), _szAlgo(params.szAlgo), _szInterpAlgo(params.szInterpAlgo)
        {
            // Validation iterations
            if (params.iterations <= 0) {
                throw std::invalid_argument("Iterations must be greater than 0");
            }
            _iterations = params.iterations;

            // Create compressor objects
            _compressor.push_back(new TrunkCompressor(_precision, _trunkCompressionLevel, params.debug));
            _compressor.push_back(new SZCompressor(_precision, _szErrorBoundMode, _szAlgo, _szInterpAlgo, params.debug));
            _compressor.push_back(new SZZlibCompressor(_precision, _trunkCompressionLevel, _szErrorBoundMode, _szAlgo, _szInterpAlgo, params.debug));
        }

        void run(std::vector<float>& data, int iterations=-1) {
            // Set number of iterations
            if (iterations == -1) {
                iterations = _iterations;
            }

            std::vector<uint8_t> compressedData{};
            std::vector<std::vector<float>> decompressedData(NUMCOMPRESSORS);

            _originalDataSize = data.size() * sizeof(float);

            // Sort data if requested
            if (_sortData) {
                std::sort(data.begin(), data.end());
            }
            
            for (int compressor{TRUNK}; compressor <= SZ; compressor++) {
                if ((compressor == TRUNK && !_doTrunk) || (compressor == SZ && !_doSZ)) {
                    continue;
                }
                
                for (int i{0}; i < iterations; ++i) {
                    compressedData.clear();
                    _start = std::chrono::high_resolution_clock::now();
                    compressedData = _compressor[compressor]->compress(data);
                    _end = std::chrono::high_resolution_clock::now();
                    _compressionTime[compressor] += std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
                }

                // Average over iterations
                _compressionTime[compressor] /= iterations;

                // Store compressed data size
                _compressedDataSize[compressor] = compressedData.size();

                // Calculate compression ratio
                _compressionRatio[compressor] = static_cast<double>(_originalDataSize) / static_cast<double>(_compressedDataSize[compressor]);

                // Decompress data
                for (int i{0}; i < iterations; ++i) {
                    _start = std::chrono::high_resolution_clock::now();
                    decompressedData[compressor] = _compressor[compressor]->decompress(compressedData, data.size());
                    _end = std::chrono::high_resolution_clock::now();
                    _decompressionTime[compressor] += std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
                }

                // Average over iterations
                _decompressionTime[compressor] /= iterations;

                // Calculate average relative error
                _avgRelativeError[compressor] = 0;
                for (size_t i{0}; i < data.size(); ++i) {
                    _avgRelativeError[compressor] += std::abs((data[i] - decompressedData[compressor][i]) / data[i]);
                }
                _avgRelativeError[compressor] /= data.size();
            }
        }

        std::string generateReport() {
            std::string report{};

            for (int compressor{TRUNK}; compressor <= SZ; compressor++) {
                if ((compressor == TRUNK && !_doTrunk) || (compressor == SZ && !_doSZ)) {
                    continue;
                }

                report += std::format("{},",(compressor == CompressorBench::TRUNK ? "Trunk" : "SZ"));
                report += std::format("{},", _iterations);
                report += std::format("{},", _dataName);
                report += std::format("{},", _precision);
                report += std::format("{},", _trunkCompressionLevel);
                report += std::format("{},", _szErrorBoundMode);
                report += std::format("{},", _szAlgo);
                report += std::format("{},", _szInterpAlgo);
                report += std::format("{},", _compressionTime[compressor]);
                report += std::format("{},", _decompressionTime[compressor]);
                report += std::format("{},", _compressionRatio[compressor]);
                report += std::format("{},", _compressedDataSize[compressor]);
                report += std::format("{},", _originalDataSize);
                report += std::format("{}", _avgRelativeError[compressor]);
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
        bool _doTrunk;
        bool _doSZ;
        bool _sortData;
        int _iterations;
        int _precision;
        std::string _dataName;

        std::vector<MyCompressor*> _compressor;

        int _trunkCompressionLevel;

        int _szErrorBoundMode;
        int _szAlgo;
        int _szInterpAlgo;

        size_t _originalDataSize;
        size_t _compressedDataSize[NUMCOMPRESSORS];
        double _compressionTime[NUMCOMPRESSORS];
        double _decompressionTime[NUMCOMPRESSORS];
        double _compressionRatio[NUMCOMPRESSORS];
        double _avgRelativeError[NUMCOMPRESSORS];

        std::chrono::high_resolution_clock::time_point _start;
        std::chrono::high_resolution_clock::time_point _end;

};

#endif // COMPRESSOR_BENCH_HPP