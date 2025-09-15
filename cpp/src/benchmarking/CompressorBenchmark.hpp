
// /**
//  * @file CompressorBenchmark.hpp
//  * @brief Classes and structures for benchmarking data compressors.
//  */
// #pragma once

// #include <string>
// #include <vector>
// #include <memory>
// #include <map>
// #include <chrono>
// #include <fstream>

// #include "../compressors/Compressor.hpp"


// /**
//  * @struct BenchmarkResult
//  * @brief Structure to hold results from a single compressor benchmark run.
//  *
//  * Members:
//  *   compressorConfig - Compressor configuration key-value pairs
//  *   benchmarkMeta    - Metadata for the benchmark run
//  *   benchmarkStats   - Numeric statistics from the benchmark
//  */
// struct BenchmarkResult {
//     std::map<std::string, std::string> compressorConfig;
//     std::map<std::string, std::string> benchmarkMeta;
//     std::map<std::string, double> benchmarkStats;
// };


// /**
//  * @class CompressorBenchmark
//  * @brief Class for running and recording benchmarks of data compressors.
//  */
// class CompressorBenchmark {
// public:
//     /**
//      * @brief Construct a CompressorBenchmark.
//      * @param compressor Shared pointer to compressor to benchmark.
//      * @param data Uncompressed data to use for benchmarking.
//      * @param outputCSV Output CSV file for results.
//      * @param iterations Number of benchmark iterations (default: 1).
//      */
//     CompressorBenchmark(std::shared_ptr<Compressor> compressor, const UncompressedData& data, const std::string& outputCSV, int iterations = 1)
//         : compressor_(std::move(compressor)), data_(data), iterations_(iterations), outputCSV_(outputCSV) {}

//     /**
//      * @brief Run the benchmark and record results.
//      * @param writeCSVHeader Write CSV header on first iteration.
//      * @param writeOnIterationFinish Write results after each iteration.
//      * @return DecompressedData from the last run.
//      */
//     DecompressedData run(bool writeCSVHeader = false, bool writeOnIterationFinish = true);

// private:
//     std::shared_ptr<Compressor> compressor_; ///< Compressor to benchmark
//     const UncompressedData data_;            ///< Data to compress
//     int iterations_ = 1;                     ///< Number of benchmark iterations
//     BenchmarkResult lastResult_;             ///< Last benchmark result
//     std::vector<BenchmarkResult> results_;   ///< All benchmark results
//     std::string outputCSV_;                  ///< Output CSV file
//     std::string outputROOT_;                 ///< Output ROOT file (unused)

//     /**
//      * @brief Get a CSV line for a benchmark result.
//      * @param result BenchmarkResult to format.
//      * @return CSV-formatted string.
//      */
//     std::string getCSVLine(const BenchmarkResult& result) const;

//     /** Write CSV header. */
//     void writeCSVHeader();
//     /** Write all results to CSV. */
//     void writeResultsToCSV();
//     /** Write last result to CSV. */
//     void writeLastResultToCSV();
// };