#include <iostream>
#include <chrono>
#include <functional>
#include <string>
#include <map>
#include <format>

#include "lib_utils.hpp"

using Compressor =  std::function<CompressorOut(const CompressorIn&, const CompressorParams&)>;
using Decompressor = std::function<DecompressorOut(const DecompressorIn&, const size_t& decompressedSize)>;

std::ofstream openCSV(std::string filename, const CompressorParams& params) {
    // Tag file with timestamp to avoid overwriting
    std::string timestamp{std::format("{}_{}.csv", filename, std::chrono::system_clock::now().time_since_epoch().count())};
    std::ofstream file(timestamp);
    if (!file.is_open()) {
        throw std::runtime_error("openCSV: failed to open file " + filename);
    }

    // Write header
    file << "system,";
    file << "timestamp,";
    file << "name,";
    for (const auto& param : params) {
        file << param.first << ",";
    }
    file << "data size (bytes),";
    file << "compression_time (ms),";
    file << "decompression_time (ms),";
    file << "compression_ratio,";
    file << "compression_error_avg,";
    file << "compression_error_min,";
    file << "compression_error_max" << std::endl;
    return file;
}

class CompressorBenchmark {
    public:
        // Constructor
        CompressorBenchmark(const std::string& name, int iterations,
                            Compressor compressor, Decompressor decompressor,
                            CompressorParams params,
                            const CompressorIn& data)
            : name_{name}, iterations_{iterations}, compressor_{compressor},
            decompressor_{decompressor}, compressorParams_{params}, uncompressedData_{data} {}

        // Run the benchmark
        void runBenchmark() {
            // Run compressor to get compression time and compressed data
            compRun_();

            // Run decompressor to get decompression time and decompressed data
            decompRun_();

            // Calculate compression ratio
            // This is the size of the uncompressed data divided by the size of the compressed data
            double uncompressedBytes{static_cast<double>(uncompressedData_.size() * sizeof(float))};
            double compressedBytes{static_cast<double>(compressedData_.size() * sizeof(uint8_t))};
            compressionRatio_ = uncompressedBytes / compressedBytes;

            // Calculate compression error
            calculateError_();

            // Print results
            std::cout << std::format("[ {} ] Compression time: {} ms", name_, compressTime_.count()) << std::endl;
            std::cout << std::format("[ {} ] Decompression time: {} ms", name_, decompressTime_.count()) << std::endl;
            std::cout << std::format("[ {} ] Compression ratio: {}", name_, compressionRatio_) << std::endl;
            std::cout << std::format("[ {} ] Compression error (avg): {}", name_, compressionErrorAvg_) << std::endl;
            std::cout << std::format("[ {} ] Compression error (min): {}", name_, compressionErrorMin_) << std::endl;
            std::cout << std::format("[ {} ] Compression error (max): {}", name_, compressionErrorMax_) << std::endl;
        }

        // Write to csv
        void writeCSVRow(std::ofstream& file) {// Write data
            // Write test metadata
            file << getHost() << ",";
            file << timestamp() << ",";
            file << name_ << ",";

            for (const auto& param : compressorParams_) {
                file << param.second << ",";
            }

            file << std::format("{}", uncompressedData_.size() * sizeof(float)) << ",";

            // Write times as milliseconds
            file << std::format("{:.17}", compressTime_.count()) << ",";
            file << std::format("{:.17}", decompressTime_.count()) << ",";

            // Write calculated values
            file << std::format("{:.17}", compressionRatio_) << ",";
            file << std::format("{:.17}", compressionErrorAvg_) << ",";
            file << std::format("{:.17}", compressionErrorMin_) << ",";
            file << std::format("{:.17}", compressionErrorMax_) << std::endl;

            file.flush();
        }

    private:
        std::string name_{};
        int iterations_{};

        Compressor compressor_{};
        CompressorParams compressorParams_{};
        Decompressor decompressor_{};

        CompressorIn uncompressedData_{};
        CompressorOut compressedData_{};
        DecompressorOut decompressedData_{};

        std::chrono::duration<double> compressTime_{};
        std::chrono::duration<double> decompressTime_{};

        double compressionRatio_{};
        double compressionErrorAvg_{};
        double compressionErrorMin_{};
        double compressionErrorMax_{};

        void compRun_() {
            double compressionTime{0.0};

            for (int i = 0; i < iterations_; ++i) {
                std::cout << std::format("[ {} ] [ {} ] Compression iteration: {} / {}", timestamp(), name_, i + 1, iterations_) << std::endl;

                // Run compressor
                auto start = std::chrono::high_resolution_clock::now();
                compressedData_ = compressor_(uncompressedData_, compressorParams_);
                auto end = std::chrono::high_resolution_clock::now();
                
                // Record time
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                compressionTime += duration.count();
            }

            // Return average of times
            compressTime_ = std::chrono::duration<double>(compressionTime / iterations_);
        }

        void decompRun_() {
            double decompressionTime{0.0};

            for (int i = 0; i < iterations_; ++i) {
                // Clear decompressedData_ to avoid memory issues
                decompressedData_.clear();

                std::cout << std::format("[ {} ] [ {} ] Decompression iteration: {} / {}", timestamp(), name_, i + 1, iterations_) << std::endl;

                // Run decompressor
                auto start = std::chrono::high_resolution_clock::now();
                decompressedData_ = decompressor_(compressedData_, uncompressedData_.size());
                auto end = std::chrono::high_resolution_clock::now();
                
                // Record time
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                decompressionTime += duration.count();
            }

            // Return average of times
            decompressTime_ = std::chrono::duration<double>(decompressionTime / iterations_);
        }

        void calculateError_() {
            // Calculate error
            std::vector<double> errors{};
            for (size_t i = 0; i < uncompressedData_.size(); ++i) {
                double error{std::abs(uncompressedData_[i] - decompressedData_[i])};
                errors.push_back(error);
            }

            // Calculate average, min, max error
            compressionErrorAvg_ = std::accumulate(errors.begin(), errors.end(), 0.0) / errors.size();
            compressionErrorMin_ = *std::min_element(errors.begin(), errors.end());
            compressionErrorMax_ = *std::max_element(errors.begin(), errors.end());
        }
};