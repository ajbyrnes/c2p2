#include <iostream>
#include <format>

#include "CompressorBenchmark.hpp"
#include "../utils/utils.hpp"

void CompressorBenchmark::run(bool report) {
    for (int i = 0; i < iterations_; ++i) {
        auto startCompression = std::chrono::high_resolution_clock::now();
        CompressedData compressedData = compressor_->compress(data_);
        auto endCompression = std::chrono::high_resolution_clock::now();

        auto startDecompression = std::chrono::high_resolution_clock::now();
        std::vector<float> decompressedData = compressor_->decompress(compressedData);
        auto endDecompression = std::chrono::high_resolution_clock::now();

        double compressionTimeMs = std::chrono::duration<double, std::milli>(endCompression - startCompression).count();
        double decompressionTimeMs = std::chrono::duration<double, std::milli>(endDecompression - startDecompression).count();
        double compressionRatio = static_cast<double>(compressedData.data.size()) / (data_.data.size() * sizeof(float));

        results_.push_back({
            .compressorName = compressor_->toString(),
            .compressorConfig = compressor_->getConfig(),

            .timestamp = getTimestamp(),
            .inputSize = static_cast<int>(data_.data.size()) * sizeof(float),
            .dataName = data_.dataName,

            .compressionRatio = compressionRatio,
            .compressionTimeMs = compressionTimeMs,
            .decompressionTimeMs = decompressionTimeMs
        });
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
    outputStream_ << "host,timestamp,inputSize,dataName,compressionRatio,compressionTimeMs,decompressionTimeMs,";

    // Write header for compressor info
    outputStream_ << "compressor,";

    std::map<std::string, std::string> config = compressor_->getConfig();
    for (const auto& [key, value] : config) {
        outputStream_ << key << ",";
    }

    // Close the output stream
    outputStream_.close();
}

std::string CompressorBenchmark::getCSVLine(const BenchmarkResult& result) const {
    std::string line = std::format("{},{},{},{},{:.10f},{:.10f},{:.10f},{},",
        getHost(),
        result.timestamp,
        result.inputSize,
        result.dataName,
        result.compressionRatio,
        result.compressionTimeMs,
        result.decompressionTimeMs,
        result.compressorName
    );

    for (const auto& [key, value] : result.compressorConfig) {
        line += std::format("{},", value);
    }

    return line;
}

void CompressorBenchmark::writeLastResultToCSV() {
    if (results_.empty()) {
        throw std::runtime_error("No benchmark results to write");
    }

    // Open output stream in append mode
    std::ofstream outputStream_;
    outputStream_.open(outputFile_, std::ios::out | std::ios::app);

    if (!outputStream_.is_open()) {
        throw std::runtime_error("Failed to open output file for benchmark results");
    }

    const auto& result = results_.back();
    outputStream_ << getCSVLine(results_.back()) << std::endl;
}

void CompressorBenchmark::writeResultsToCSV() {
    // Open output stream in append mode
    std::ofstream outputStream_;
    outputStream_.open(outputFile_, std::ios::out | std::ios::app);

    if (!outputStream_.is_open()) {
        throw std::runtime_error("Failed to open output file for benchmark results");
    }
    // Write benchmark results
    for (const auto& result : results_) {
        outputStream_ << getCSVLine(result) << std::endl;
    }

    // Close the output stream
    outputStream_.close();
    std::cout << std::format("[{}] Benchmark results written to '{}'", getTimestamp(), outputFile_) << std::endl;
}
