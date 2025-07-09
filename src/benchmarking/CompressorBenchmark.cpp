#include <iostream>

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

void CompressorBenchmark::writeResultsToCSV(std::string outputFile) const {
    // Open output file
    std::ofstream ofs(outputFile);
    if (!ofs.is_open()) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return;
    }

    // Write header for compressor info
    ofs << "compressor,";

    std::map<std::string, std::string> config = compressor_->getConfig();
    for (const auto& [key, value] : config) {
        ofs << key << ",";
    }

    // Write header for benchmark results
    ofs << "host,timestamp,inputSize,dataName,compressionRatio,compressionTimeMs,decompressionTimeMs\n";

    std::string host = getHost();

    // Write benchmark results
    for (const auto& result : results_) {
        // Write compressor info
        ofs << std::format("{},", result.compressorName);
        for (const auto& [key, value] : result.compressorConfig) {
            ofs << std::format("{},", value);
        }

        // Write benchmark info
        ofs << std::format("{},", host)
            << std::format("{},", result.timestamp)
            << std::format("{},", result.inputSize)
            << std::format("{},", result.dataName)
            << std::format("{:.10f},", result.compressionRatio)
            << std::format("{:.10f},", result.compressionTimeMs)
            << std::format("{:.10f}", result.decompressionTimeMs) << std::endl;
    }
}
