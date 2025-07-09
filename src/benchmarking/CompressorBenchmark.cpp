#include <iostream>
#include <format>
#include <cmath>

#include "CompressorBenchmark.hpp"
#include "../utils/utils.hpp"

void CompressorBenchmark::run(bool writeOnIterationFinish) {
    for (int i = 0; i < iterations_; ++i) {
        std::cout << std::format("[{}] Running benchmark iteration {}/{}", getTimestamp(), i + 1, iterations_) << std::endl;

        auto startCompression = std::chrono::high_resolution_clock::now();
        CompressedData compressedData = compressor_->compress(data_);
        auto endCompression = std::chrono::high_resolution_clock::now();

        auto startDecompression = std::chrono::high_resolution_clock::now();
        std::vector<float> decompressedData = compressor_->decompress(compressedData);
        auto endDecompression = std::chrono::high_resolution_clock::now();

        double compressionTimeMs = std::chrono::duration<double, std::milli>(endCompression - startCompression).count();
        double decompressionTimeMs = std::chrono::duration<double, std::milli>(endDecompression - startDecompression).count();

        // Calculate ratio of original data size to compressed data size
        double compressionRatio =  (data_.data.size() * sizeof(float)) / static_cast<double>(compressedData.data.size());

        // Calculate RMSE of decompressed data wrt original data
        double rmse = 0.0;
        if (decompressedData.size() == data_.data.size()) {
            for (size_t j = 0; j < data_.data.size(); ++j) {
                double diff = static_cast<double>(decompressedData[j]) - static_cast<double>(data_.data[j]);
                rmse += diff * diff;
            }
            rmse = std::sqrt(rmse / static_cast<double>(data_.data.size()));
        }

        // Calculate average point-wise relative error
        double avgRelativeError = 0.0;
        if (decompressedData.size() == data_.data.size()) {
            for (size_t j = 0; j < data_.data.size(); ++j) {
                double originalValue = static_cast<double>(data_.data[j]);
                if (originalValue != 0.0) {
                    avgRelativeError += std::abs((static_cast<double>(decompressedData[j]) - originalValue) / originalValue);
                } else {
                    // Handle case where original value is zero to avoid division by zero
                    if (decompressedData[j] != 0.0) {
                        avgRelativeError += std::abs(static_cast<double>(decompressedData[j]));
                    }
                }
            }
            avgRelativeError /= static_cast<double>(data_.data.size());
        }

        // Calculate maximum point-wise relative error
        double maxRelativeError = 0.0;
        if (decompressedData.size() == data_.data.size()) {
            for (size_t j = 0; j < data_.data.size(); ++j) {
                double originalValue = static_cast<double>(data_.data[j]);
                if (originalValue != 0.0) {
                    double relativeError = std::abs((static_cast<double>(decompressedData[j]) - originalValue) / originalValue);
                    if (relativeError > maxRelativeError) {
                        maxRelativeError = relativeError;
                    }
                } else {
                    // Handle case where original value is zero to avoid division by zero
                    if (decompressedData[j] != 0.0) {
                        maxRelativeError = std::max(maxRelativeError, std::abs(static_cast<double>(decompressedData[j])));
                    }
                }
            }
        }

        lastResult_ = {
            .compressorName = compressor_->toString(),
            .compressorConfig = compressor_->getConfig(),

            .timestamp = getTimestamp(),
            .inputSize = static_cast<int>(data_.data.size()) * sizeof(float),
            .dataName = data_.dataName,

            .compressionRatio = compressionRatio,
            .compressionTimeMs = compressionTimeMs,
            .decompressionTimeMs = decompressionTimeMs,
            .rmse = rmse,
            .avgRelativeError = avgRelativeError,
            .maxRelativeError = maxRelativeError
        };

        writeLastResultToCSV();
    }
}

void CompressorBenchmark::writeCSVHeader() {
    // Open output stream in append mode
    std::ofstream outputStream_;
    outputStream_.open(outputFile_, std::ios::out | std::ios::app);

    if (!outputStream_.is_open()) {
        throw std::runtime_error("Failed to open output file for benchmark results");
    }

    // Write header for benchmark results
    outputStream_ << "host,timestamp,inputSize,dataName,compressionRatio,compressionTimeMs,decompressionTimeMs,rmse,avgRelativeError,maxRelativeError";

    // Write header for compressor info
    outputStream_ << "compressor,";

    std::map<std::string, std::string> config = compressor_->getConfig();
    for (const auto& [key, value] : config) {
        outputStream_ << key << ",";
    }

    outputStream_ << std::endl;

    // Close the output stream
    outputStream_.close();
}

std::string CompressorBenchmark::getCSVLine(const BenchmarkResult& result) const {
    std::string line = std::format("{},{},{},{},{:.10f},{:.10f},{:.10f},{:.10f},{:.10f},{:.10f},{},",
        getHost(),
        result.timestamp,
        result.inputSize,
        result.dataName,
        result.compressionRatio,
        result.compressionTimeMs,
        result.decompressionTimeMs,
        result.rmse,
        result.avgRelativeError,
        result.maxRelativeError,
        result.compressorName
    );

    for (const auto& [key, value] : result.compressorConfig) {
        line += std::format("{},", value);
    }

    return line;
}

void CompressorBenchmark::writeLastResultToCSV() {
    // if (results_.empty()) {
    //     throw std::runtime_error("No benchmark results to write");
    // }

    // Open output stream in append mode
    std::ofstream outputStream_;
    outputStream_.open(outputFile_, std::ios::out | std::ios::app);

    if (!outputStream_.is_open()) {
        throw std::runtime_error("Failed to open output file for benchmark results");
    }

    // const auto& result = results_.back();
    // outputStream_ << getCSVLine(results_.back()) << std::endl;
    outputStream_ << getCSVLine(lastResult_) << std::endl;

    // Close the output stream
    outputStream_.close();
    std::cout << std::format("[{}] Last benchmark result written to '{}'", getTimestamp(), outputFile_) << std::endl;
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
