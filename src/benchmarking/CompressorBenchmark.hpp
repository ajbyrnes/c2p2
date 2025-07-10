#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <fstream>

#include "../compressors/Compressor.hpp"

struct BenchmarkResult {
    std::string compressorName; 
    std::map<std::string, std::string> compressorConfig;
    
    std::string timestamp;
    int inputSize;      // Bytes
    int outputSize;     // Bytes
    std::string fileName;
    std::string dataName;

    double compressionRatio;
    double compressionTimeMs;
    double decompressionTimeMs;
    double rmse;
    double avgRelativeError;
    double maxRelativeError;
};

class CompressorBenchmark {
public:
    CompressorBenchmark(std::shared_ptr<Compressor> compressor, const UncompressedData& data, const std::string& outputFile, int iterations = 1)
        : compressor_(std::move(compressor)), data_(data), iterations_(iterations), outputFile_(outputFile) {}

    void run(bool writeOnIterationFinish = true);
    void writeCSVHeader();
    void writeResultsToCSV();
    void writeLastResultToCSV();

private:
    std::shared_ptr<Compressor> compressor_;
    const UncompressedData data_;
    int iterations_ = 1; // Default number of iterations for benchmarks
    BenchmarkResult lastResult_;
    // std::vector<BenchmarkResult> results_; // Store results for multiple iterations
    std::string outputFile_;

    std::string getCSVLine(const BenchmarkResult& result) const;
};