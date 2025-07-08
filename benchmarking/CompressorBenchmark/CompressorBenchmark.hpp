#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>


// Benchmark result structure
struct BenchmarkResult {
    std::string compressorName;
    double compressionRatio;
    double compressionTimeMs;
    double decompressionTimeMs;
    double maxAbsoluteError;
    double meanSquaredError;
};