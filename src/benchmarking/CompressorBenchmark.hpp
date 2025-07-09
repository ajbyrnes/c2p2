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
    int inputSize;
    std::string dataName;

    double compressionRatio;
    double compressionTimeMs;
    double decompressionTimeMs;
};

class CompressorBenchmark {
public:
    CompressorBenchmark(std::shared_ptr<Compressor> compressor, const UncompressedData& data, int iterations = 1)
        : compressor_(std::move(compressor)), data_(data), iterations_(iterations) {}

    void run(bool report=false);
    void writeResultsToCSV(std::string outputFile) const;

private:
    std::shared_ptr<Compressor> compressor_;
    UncompressedData data_;
    int iterations_ = 1; // Default number of iterations for benchmarks
    std::vector<BenchmarkResult> results_; // Store results for multiple iterations
};