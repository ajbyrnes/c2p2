#ifndef COMPRESSOR_BENCH_HPP
#define COMPRESSOR_BENCH_HPP

#include <format>
#include <fstream>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <unistd.h>

#include "utils.hpp"
#include "TrunkCompressor.hpp"
#include "SZCompressor.hpp"
// #include "SZZlibCompressor.hpp"

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
    std::string treeName;
    std::string branchName;

    int seed;
    float mean;
    float stddev;

    int trunkCompressionLevel;

    int szErrorBoundMode;
    int szAlgo;
    int szInterpAlgo;

    std::string reportType;
};

struct TimeCollector {
    double user;
    double system;
    double real;
};

BenchmarkParams parseArguments(int argc, char* argv[]) {
    // Set default parameters
    BenchmarkParams params;

    params.doTrunk = false;
    params.doSZ = true;
    params.sortData = false;

    params.iterations = 5;
    params.precision = 3;
    params.debug = false;

    params.dataMB = 0;
    params.dataName = "root";
    params.sourceFile = "mc_361106.Zee.1largeRjet1lep.root";
    params.treeName = "mini";
    params.branchName = "lep_pt";

    params.seed = 12345;
    params.mean = 0.0;
    params.stddev = 1.0f;

    params.trunkCompressionLevel = 9;
    params.szErrorBoundMode = SZ3::EB_REL;
    params.szAlgo = SZ3::ALGO_LORENZO_REG;
    params.szInterpAlgo = SZ3::INTERP_ALGO_LINEAR;

    params.reportType = "formatted";

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
        } else if (arg == "--reportType") {
            params.reportType = argv[++i];
        } else {
            throw std::invalid_argument("Unknown argument: " + arg);
        }
    }

    // Validate branch name
    // Name should be in floatBranches or vectorFloatBranches
    if (std::find(floatBranches.begin(), floatBranches.end(), params.branchName) == floatBranches.end() &&
        std::find(vectorFloatBranches.begin(), vectorFloatBranches.end(), params.branchName) == vectorFloatBranches.end()) {
        throw std::invalid_argument("Invalid branch name: " + params.branchName);
    }

    return params;
}

constexpr int NUMCOMPRESSORS{3};

class CompressorBench{
    public:
        enum COMPRESSOR{TRUNK, SZ};

        CompressorBench(const BenchmarkParams& params)
            :   _doSZ(params.doSZ), _doTrunk(params.doTrunk),
                _dataName(params.dataName), _precision(params.precision), _debug(params.debug),
                _trunkCompressionLevel(params.trunkCompressionLevel),
                _szErrorBoundMode(params.szErrorBoundMode), _szAlgo(params.szAlgo), _szInterpAlgo(params.szInterpAlgo)
        {
            // Validation iterations
            if (params.iterations <= 0) {
                throw std::invalid_argument("Iterations must be greater than 0");
            }
            _iterations = params.iterations;

            if (_dataName == "root") {
                // Validate source file
                if (params.sourceFile.empty()) {
                    throw std::invalid_argument("Source file must be specified");
                }
                _sourceFile = params.sourceFile;

                // Validate tree name
                if (params.treeName.empty()) {
                    throw std::invalid_argument("Tree name must be specified");
                }
                _treeName = params.treeName;

                // Validate branch name
                if (params.branchName.empty()) {
                    throw std::invalid_argument("Branch name must be specified");
                }
                _branchName = params.branchName;
            }

            // Create compressor objects
            _compressor.push_back(new TrunkCompressor(_precision, _trunkCompressionLevel, _debug));
            _compressor.push_back(new SZCompressor(_precision, _szErrorBoundMode, _szAlgo, _szInterpAlgo, _debug));
            // _compressor.push_back(new SZZlibCompressor(_precision, _trunkCompressionLevel, _szErrorBoundMode, _szAlgo, _szInterpAlgo, params.debug));
        }

        void run(std::vector<float>& data, int iterations=-1) {
            // Set number of iterations
            if (iterations == -1) {
                iterations = _iterations;
            }

            std::vector<uint8_t> compressedData{};
            std::vector<std::vector<float>> decompressedData(NUMCOMPRESSORS);

            _originalDataSize = data.size() * sizeof(float);
            
            for (int compressor{TRUNK}; compressor <= SZ; compressor++) {
                if ((compressor == TRUNK && !_doTrunk) || (compressor == SZ && !_doSZ)) {
                    continue;
                }
                
                for (int i{0}; i < iterations; ++i) {
                    // Clear compressed data, just to be safe
                    compressedData.clear();

                    // Start timing
                    _startReal = std::chrono::high_resolution_clock::now();
                    _getCPUTime(_startUser, _startSystem);
                    _startMemory = _getMemoryUsage();

                    // Perform compression
                    compressedData = _compressor[compressor]->compress(data);
                    
                    // Stop timing
                    _endReal = std::chrono::high_resolution_clock::now();
                    _getCPUTime(_endUser, _endSystem);
                    _endMemory = _getMemoryUsage();

                    // Calculate time spent
                    _compressionTime[compressor].real += std::chrono::duration_cast<std::chrono::milliseconds>(_endReal - _startReal).count();
                    _compressionTime[compressor].user += _endUser - _startUser;
                    _compressionTime[compressor].system += _endSystem - _startSystem;

                    // Calculate memory usage
                    _compressionMemory[compressor] = _endMemory - _startMemory;
                }

                // Average time over iterations
                _compressionTime[compressor].real /= iterations;
                _compressionTime[compressor].user /= iterations;
                _compressionTime[compressor].system /= iterations;

                // Average memory usage over iterations
                _compressionMemory[compressor] /= iterations;

                // Store compressed data size
                _compressedDataSize[compressor] = compressedData.size();

                // Calculate compression ratio
                _compressionRatio[compressor] = static_cast<double>(_originalDataSize) / static_cast<double>(_compressedDataSize[compressor]);

                // Decompress data
                for (int i{0}; i < iterations; ++i) {
                    // Start timing
                    _startReal = std::chrono::high_resolution_clock::now();
                    _getCPUTime(_startUser, _startSystem);
                    _startMemory = _getMemoryUsage();

                    // Perform decompression
                    decompressedData[compressor] = _compressor[compressor]->decompress(compressedData, data.size());

                    // Stop timing
                    _endReal = std::chrono::high_resolution_clock::now();
                    _getCPUTime(_endUser, _endSystem);
                    _endMemory = _getMemoryUsage();

                    // Calculate time spent
                    _decompressionTime[compressor].real += std::chrono::duration_cast<std::chrono::milliseconds>(_endReal - _startReal).count();
                    _decompressionTime[compressor].user += _endUser - _startUser;
                    _decompressionTime[compressor].system += _endSystem - _startSystem;

                    // Calculate memory usage
                    _decompressionMemory[compressor] = _endMemory - _startMemory;
                }

                // Average time over iterations
                _decompressionTime[compressor].real /= iterations;
                _decompressionTime[compressor].user /= iterations;
                _decompressionTime[compressor].system /= iterations;

                // Average memory usage over iterations
                _decompressionMemory[compressor] /= iterations;

                // Calculate average relative error
                _avgRelativeError[compressor] = 0;
                for (size_t i{0}; i < data.size(); ++i) {
                    if (data[i]) {
                        _avgRelativeError[compressor] += std::abs((data[i] - decompressedData[compressor][i]) / data[i]);
                    }
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

                report += std::format("Compressor: {}\n", (compressor == CompressorBench::TRUNK ? "Trunk" : "SZ"));
                report += std::format("Iterations: {}\n", _iterations);

                report += std::format("Data name: {}\n", _dataName);
                if (_dataName == "root") {
                    report += std::format("Source file: {}\n", _sourceFile);
                    report += std::format("Tree name: {}\n", _treeName);
                    report += std::format("Branch name: {}\n", _branchName);
                }

                report += std::format("Precision: {}\n", _precision);

                if ((compressor == TRUNK && _doTrunk)) {
                    report += std::format("Trunk compression level: {}\n", _trunkCompressionLevel);
                }
                
                if ((compressor == SZ && _doSZ)) {
                    report += std::format("SZ error bound mode: {}\n", _szErrorBoundMode);
                    report += std::format("SZ algorithm: {}\n", _szAlgo);
                    report += std::format("SZ interpolation algorithm: {}\n", _szInterpAlgo);
                }

                report += std::format("Average compression time: {:.2f} ms (user: {:.2f} ms, system: {:.2f} ms)\n",
                    _compressionTime[compressor].real, _compressionTime[compressor].user, _compressionTime[compressor].system);

                report += std::format("Average decompression time: {:.2f} ms (user: {:.2f} ms, system: {:.2f} ms)\n",
                    _decompressionTime[compressor].real, _decompressionTime[compressor].user, _decompressionTime[compressor].system);

                report += std::format("Average compression memory: {} KB\n", _compressionMemory[compressor]);

                report += std::format("Original data size: {} bytes\n", _originalDataSize);
                report += std::format("Compressed data size: {} bytes\n", _compressedDataSize[compressor]);
                report += std::format("Compression ratio: {:.2f}\n", _compressionRatio[compressor]);

                report += std::format("Average relative error: {:.6f}\n\n", _avgRelativeError[compressor]);
            }

            return report;
        }

        // Getters
        int getIterations() const {
            return _iterations;
        }
        
        TimeCollector getCompressionTime(const COMPRESSOR compressor) const {
            return _compressionTime[compressor];
        }

        TimeCollector getDecompressionTime(const COMPRESSOR compressor) const {
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
                _compressionTime[compressor] = TimeCollector{0, 0, 0};
                _decompressionTime[compressor] = TimeCollector{0, 0, 0};
                _compressedDataSize[compressor] = 0;
                _compressionRatio[compressor] = 0;
                _compressionMemory[compressor] = 0;
                _decompressionMemory[compressor] = 0;
            }
            _originalDataSize = 0;
        }

    private:        
        bool _doTrunk;
        bool _doSZ;

        int _iterations;
        int _precision;
        bool _debug;
      
        std::string _dataName;
        std::string _sourceFile;
        std::string _treeName;
        std::string _branchName;

        std::vector<MyCompressor*> _compressor;

        int _trunkCompressionLevel;

        int _szErrorBoundMode;
        int _szAlgo;
        int _szInterpAlgo;

        size_t _originalDataSize;
        size_t _compressedDataSize[NUMCOMPRESSORS];
        double _compressionRatio[NUMCOMPRESSORS];

        double _avgRelativeError[NUMCOMPRESSORS];

        TimeCollector _compressionTime[NUMCOMPRESSORS];
        TimeCollector _decompressionTime[NUMCOMPRESSORS];
        
        std::chrono::high_resolution_clock::time_point _startReal;
        std::chrono::high_resolution_clock::time_point _endReal;
        double _startUser;
        double _endUser;
        double _startSystem;
        double _endSystem;

        double _compressionMemory[NUMCOMPRESSORS];
        double _decompressionMemory[NUMCOMPRESSORS];
        size_t _startMemory;
        size_t _endMemory;

        // Get user and system CPU time for this process from /proc/self/stat
        // Time is measured in milliseconds
        void _getCPUTime(double& user, double& system) {
            std::ifstream stat("/proc/self/stat");
            if (!stat.is_open()) {
                throw std::runtime_error("Failed to open /proc/self/stat");
            }
            
            // Create string stream to read from stat file
            std::string line;
            std::getline(stat, line);
            std::istringstream iss(line);
            std::string token;

            // Skip first 13 fields
            for (int i = 0; i < 13; ++i) {
                std::getline(iss, token, ' ');
            }

            // Read user and system jiffies
            long utime_jiffies{0};
            long stime_jiffies{0};
            iss >> utime_jiffies >> stime_jiffies;

            // Convert jiffies to seconds
            long ticks_per_second{sysconf(_SC_CLK_TCK)};
            if (ticks_per_second <= 0) {
                throw std::runtime_error("Failed to get ticks per second");
            }

            user = (1000.0 * utime_jiffies) / ticks_per_second;
            system = (1000.0 * stime_jiffies) / ticks_per_second;

            // Close file
            stat.close();
        }

        // Get memory usage for this process from /proc/self/status
        // Memory is measured in kilobytes
        size_t _getMemoryUsage() {
            // Open /proc/self/status
            std::ifstream status("/proc/self/status");
            if (!status.is_open()) {
                throw std::runtime_error("Failed to open /proc/self/status");
            }

            // Read lines until we find VmRSS
            std::string line;
            while (std::getline(status, line)) {
                if (line.rfind("VmRSS:", 0) == 0) {     // Found line with VmRSS
                    std::istringstream iss(line);

                    std::string token;
                    size_t value;
                    std::string unit;

                    iss >> token >> value >> unit; // Read VmRSS value and unit
                    return value;
                }
            }

            return 0;
        }
};

#endif // COMPRESSOR_BENCH_HPP