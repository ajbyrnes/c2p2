#include <iostream>
#include <format>
#include <cmath>
#include <numeric>

#include "CompressorBenchmark.hpp"
#include "../utils/utils.hpp"

// All of the compressors are deterministic
// The only purpose of the iterations is to average over *time*
DecompressedData CompressorBenchmark::run(bool writeCSVHeader, bool writeOnIterationFinish) {
    DecompressedData decompressedData;

    for (int i = 0; i < iterations_; ++i) {
        std::cout << timeMessage(std::format("Running benchmark iteration {}/{}", i + 1, iterations_)) << std::endl;

        auto startCompression = std::chrono::high_resolution_clock::now();
        CompressedData compressedData = compressor_->compress(data_);
        auto endCompression = std::chrono::high_resolution_clock::now();

        auto startDecompression = std::chrono::high_resolution_clock::now();
        decompressedData = compressor_->decompress(compressedData);
        auto endDecompression = std::chrono::high_resolution_clock::now();

        double compressionTimeMs = std::chrono::duration<double, std::milli>(endCompression - startCompression).count();
        double decompressionTimeMs = std::chrono::duration<double, std::milli>(endDecompression - startDecompression).count();

        // Calculate ratio of original data size to compressed data size
        double compressionRatio =  (data_.data.size() * sizeof(float)) / static_cast<double>(compressedData.data.size());

        // Calculate pointwise abosolute and relative errors
        std::vector<double> absErrors, relErrors;
        absErrors.reserve(data_.numFloats);
        relErrors.reserve(data_.numFloats);
        for (size_t j = 0; j < data_.numFloats; ++j) {
            double absDiff = std::abs(data_.data[j] - decompressedData.data[j]);
            absErrors.push_back(absDiff);

            double relError = (data_.data[j] != 0.0f) ? (absDiff * 100) / std::abs(data_.data[j]) : absDiff * 100;
            relErrors.push_back(relError);
        }

        // Sort errors
        // This is necessary for quartiles and median calculations
        std::sort(absErrors.begin(), absErrors.end());
        std::sort(relErrors.begin(), relErrors.end());

        // Calculate absolute error stats

        if (absErrors.empty()) {
            throw std::runtime_error("No absolute errors calculated, check data integrity");
        }        

        double maxAbsError = *std::max_element(absErrors.begin(), absErrors.end());
        double minAbsError = *std::min_element(absErrors.begin(), absErrors.end());
        double q1AbsError = absErrors[absErrors.size() / 4];
        double q3AbsError = absErrors[3 * absErrors.size() / 4];
        double medianAbsError = absErrors[absErrors.size() / 2];
        double meanAbsError = std::accumulate(absErrors.begin(), absErrors.end(), 0.0) / data_.numFloats;
        double stdDevAbsError = std::sqrt(std::accumulate(absErrors.begin(), absErrors.end(), 0.0, [](double sum, double val) {
            return sum + (val * val);
        }) / data_.numFloats - meanAbsError * meanAbsError);

        // Calculate relative error stats

        if (relErrors.empty()) {
            throw std::runtime_error("No relative errors calculated, check data integrity");
        }

        double maxRelError = *std::max_element(relErrors.begin(), relErrors.end());
        double minRelError = *std::min_element(relErrors.begin(), relErrors.end());
        double q1RelError = relErrors[relErrors.size() / 4];
        double q3RelError = relErrors[3 * relErrors.size() / 4];
        double medianRelError = relErrors[relErrors.size() / 2];
        double meanRelError = std::accumulate(relErrors.begin(), relErrors.end(), 0.0) / data_.numFloats;
        double stdDevRelError = std::sqrt(std::accumulate(relErrors.begin(), relErrors.end(), 0.0, [](double sum, double val) {
            return sum + (val * val);
        }) / data_.numFloats - meanRelError * meanRelError);

        // Package and write results
        
        std::map<std::string, std::string> compressorConfig = compressor_->getConfig();
        std::map<std::string, std::string> benchmarkMeta = {
            {"host", getHost()},
            {"timestamp", getTimestamp()},
            {"dataFile", data_.fileName},
            {"dataName", data_.dataName},
            {"compressor", compressor_->toString()},
        };

        std::map<std::string, double> benchmarkStats = {
            {"inputSizeBytes", static_cast<double>(data_.data.size() * sizeof(float))},
            {"outputSizeBytes", static_cast<double>(compressedData.data.size())},
            {"compressionRatio", compressionRatio},
            {"compressionTimeMs", compressionTimeMs},
            {"decompressionTimeMs", decompressionTimeMs},
            {"maxAbsError", maxAbsError},
            {"minAbsError", minAbsError},
            {"q1AbsError", q1AbsError},
            {"q3AbsError", q3AbsError},
            {"medianAbsError", medianAbsError},
            {"meanAbsError", meanAbsError},
            {"stdDevAbsError", stdDevAbsError},
            {"maxRelErrorPct", maxRelError},
            {"minRelErrorPct", minRelError},
            {"q1RelErrorPct", q1RelError},
            {"q3RelErrorPct", q3RelError},
            {"medianRelErrorPct", medianRelError},
            {"meanRelErrorPct", meanRelError},
            {"stdDevRelErrorPct", stdDevRelError},
        };

        lastResult_ = {
            .compressorConfig = compressorConfig,
            .benchmarkMeta = benchmarkMeta,
            .benchmarkStats = benchmarkStats,
        };

        results_.push_back(lastResult_);

        std::cout << timeMessage(std::format("Benchmark iteration {}/{} completed", i + 1, iterations_)) << std::endl;

        if (writeCSVHeader && i == 0) {
            CompressorBenchmark::writeCSVHeader();
        }

        if (writeOnIterationFinish) {
            writeLastResultToCSV();
        }
    }

    return decompressedData;
}

void CompressorBenchmark::writeCSVHeader() {
    // Open output stream in append mode
    std::ofstream outputStream_;
    outputStream_.open(outputCSV_, std::ios::out | std::ios::app);

    if (!outputStream_.is_open()) {
        throw std::runtime_error("Failed to open output file for benchmark results");
    }

    // Write header for benchmark metadata
    for (const auto& [key, value] : lastResult_.benchmarkMeta) {
        outputStream_ << key << ",";
    }

    // Write header for compressor config
    for (const auto& [key, value] : lastResult_.compressorConfig) {
        outputStream_ << key << ",";
    }

    // Write header for benchmark stats
    for (const auto& [key, value] : lastResult_.benchmarkStats) {
        outputStream_ << key << ",";
    }

    outputStream_ << std::endl; // End the header line

    // Close the output stream
    outputStream_.close();
}

std::string CompressorBenchmark::getCSVLine(const BenchmarkResult& result) const {
    // Write benchmark metadata
    std::string line;
    for (const auto& [key, value] : result.benchmarkMeta) {
        line += std::format("{},", value);
    }

    // Write compressor config
    for (const auto& [key, value] : result.compressorConfig) {
        line += std::format("{},", value);
    }

    // Write benchmark stats
    for (const auto& [key, value] : result.benchmarkStats) {
        line += std::format("{:.10f},", value);
    }

    return line;
}

void CompressorBenchmark::writeLastResultToCSV() {
    // if (results_.empty()) {
    //     throw std::runtime_error("No benchmark results to write");
    // }

    // Open output stream in append mode
    std::ofstream outputStream_;
    outputStream_.open(outputCSV_, std::ios::out | std::ios::app);

    if (!outputStream_.is_open()) {
        throw std::runtime_error("Failed to open output file for benchmark results");
    }

    // const auto& result = results_.back();
    // outputStream_ << getCSVLine(results_.back()) << std::endl;
    outputStream_ << getCSVLine(lastResult_) << std::endl;

    // Close the output stream
    outputStream_.close();
    std::cout << std::format("[{}] Last benchmark result written to '{}'", getTimestamp(), outputCSV_) << std::endl;
}

void CompressorBenchmark::writeResultsToCSV() {
    // // Open output stream in append mode
    // std::ofstream outputStream_;
    // outputStream_.open(outputFile_, std::ios::out | std::ios::app);

    // if (!outputStream_.is_open()) {
    //     throw std::runtime_error("Failed to open output file for benchmark results");
    // }
    // // Write benchmark results
    // for (const auto& result : results_) {
    //     outputStream_ << getCSVLine(result) << std::endl;
    // }

    // // Close the output stream
    // outputStream_.close();
    // std::cout << std::format("[{}] Benchmark results written to '{}'", getTimestamp(), outputFile_) << std::endl;
}
