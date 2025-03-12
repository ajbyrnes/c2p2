#include <chrono>
#include <format>
#include <fstream>
#include <iostream>

#include "lib_sz.hpp"

// Write header to file
void writeHeader(std::ofstream& file) {
    // Write header to file
    file << "System,";
    file << "Timestamp,";
    file << "Function,";
    file << "Error Bound Mode,";
    file << "Compression Algorithm,";
    file << "Interpolation Algorithm,";
    file << "Absolute Error Bound,";
    file << "Trial,";
    file << "Compression Duration (ns),";
    file << "Decompression Duration (ns),";
    file << "Original Size,";
    file << "Compressed Size,";
    file << "Compression Ratio,";
    file << "Original Mean,";
    file << "Original RMS,";
    file << "Decompressed Mean,";
    file << "Decompressed RMS,";
    file << "Mean CE,";
    file << "RMS CE";
    file << std::endl;
}

void writeResults(std::ofstream& file, const CompressionDecompressionResults& results) {
    file << std::format("{},", results.system);
    file << std::format("{},", results.timestamp);
    file << std::format("{},", results.dataName);
    file << std::format("{},", results.errorBoundMode);
    file << std::format("{},", results.algo);
    file << std::format("{},", results.ialgo);
    file << std::format("{},", results.absErrorBound);
    file << std::format("{},", results.trial);
    file << std::format("{},", results.compressionDuration.count());
    file << std::format("{},", results.decompressionDuration.count());
    file << std::format("{},", results.originalSize);
    file << std::format("{},", results.compressedSize);
    file << std::format("{:.16f},", results.compressionRatio);
    file << std::format("{:.16f},", results.originalMean);
    file << std::format("{:.16f},", results.originalRMS);
    file << std::format("{:.16f},", results.decompressedMean);
    file << std::format("{:.16f},", results.decompressedRMS);
    file << std::format("{:.16f},", results.meanCE);
    file << std::format("{:.16f}", results.rmsCE);
    file << std::endl;
}

void test(std::ofstream& file, const std::string& host, const std::vector<float>& original, const std::string& dataName, const int& trials,
          const SZ3::EB& errorBoundMode, const SZ3::ALGO& algo, const SZ3::INTERP_ALGO& interpAlgo, const float& absErrorBound) {

    // Collect original distribution stats
    CompressionDecompressionResults results{};
    results.originalSize = original.size();
    results.system = host;
    results.dataName = dataName;

    results.errorBoundMode = errorBoundMode;
    results.algo = algo;
    results.ialgo = interpAlgo;
    results.absErrorBound = absErrorBound;

    results.originalMean = calculateMean(original);
    results.originalRMS = calculateRMS(original);

    // Print first 10 values of original data
    std::cout << "Original data: ";
    for (int i{0}; i < 10; i++) {
        std::cout << original[i] << " ";
    }
    std::cout << std::endl;

    // Test compression and decompression for each number of bits
    for (int trial{1}; trial <= trials; trial++) {
        // Update results for current trial
        results.timestamp = std::chrono::system_clock::now();
        results.trial = trial;

        // Compress data
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Compressing..." << std::endl;
        
        // Collect compression stats
        CompressionResults compResult{timedSzCompress(original, errorBoundMode, algo, interpAlgo, absErrorBound)};
        results.compressedSize = compResult.compressedData.size();
        results.compressionDuration = compResult.duration;

        // Decompress data
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Decompressing..." << std::endl;

        DecompressionResults decompResult{timedSzDecompress(compResult.compressedData, results.originalSize)};

        // Print first 10 values of decompressed data
        std::cout << "Decompressed data: ";
        for (int i{0}; i < 10; i++) {
            std::cout << decompResult.decompressedData[i] << " ";
        }
        std::cout << std::endl;

        // Collect decompressed distribution stats
        results.decompressionDuration = decompResult.duration;

        results.decompressedMean = calculateMean(decompResult.decompressedData);
        results.decompressedRMS = calculateRMS(decompResult.decompressedData);
        
        results.meanCE = std::abs(results.originalMean - results.decompressedMean);
        results.rmsCE = std::abs(results.originalRMS - results.decompressedRMS);
        
        // Calculate compression ratio
        results.compressionRatio = static_cast<double>(results.originalSize) / results.compressedSize;

        // Write results to file
        writeResults(file, results);
    }
}

int main(int argc, char* argv[]) {
    // Process args
    int MODE{0};
    int TRIALS{0};
    int DATASIZE{0};
    int EB_VAL{0};
    int ALGO_VAL{0};
    int INTERP_ALGO_VAL{0};

    if (argc != 6) {
        // Usage
        std::cout << "Usage: " << argv[0] << " <eb> <algo> <interpAlgo> <dataSize> <mode> <numTrials>" << std::endl;
        std::cout << "<eb>: Error bound mode" << std::endl;
        std::cout << "<algo>: Compression algorithm" << std::endl;
        std::cout << "<interpAlgo>: Interpolation algorithm" << std::endl;
        std::cout << "<datasize>: Size of data to compress in KB" << std::endl;
        std::cout << "<mode>:" << std::endl;
        std::cout << "\t1: Uniform Random Data" << std::endl;    
        std::cout << "\t2: Normal Random Data" << std::endl;
        std::cout << "\t3: Sorted Uniform Random Data" << std::endl;
        std::cout << "\t4: Sorted Normal Random Data" << std::endl;
        std::cout << "<numTrials>: Number of times to perform test" << std::endl;
        return EXIT_FAILURE;
    }
    else {
        EB_VAL = std::stoi(argv[1]);
        ALGO_VAL = std::stoi(argv[2]);
        INTERP_ALGO_VAL = std::stoi(argv[3]);
        DATASIZE = std::stoi(argv[4]) * KB / sizeof(float);
        // MODE = std::stoi(argv[5]);
        TRIALS = std::stoi(argv[5]);
    }

    // Determine host
    std::string host{getHost()};

    // Open file for writing
    std::string filename{std::format("func-sz-eb={}-al={}-ial={}.csv", EB_VAL, ALGO_VAL, INTERP_ALGO_VAL)};
    std::ofstream file{filename};
    writeHeader(file);

    // Test distributions
    std::vector<float> testData{generateLinearFunction(DATASIZE, 1.0, 0.0)};
    std::string dataName{std::format("linear-m={}-b={}", 1.0, 0.0)};

    std::cout << "(" << std::chrono::system_clock::now() << ") ";
    std::cout << "Generating vector with " << DATASIZE << " floats (" << (DATASIZE * sizeof(float)) / KB << " KB)..." << std::endl;

    test(file, host, testData, dataName, TRIALS, SZ3::EB(EB_VAL), SZ3::ALGO(ALGO_VAL), SZ3::INTERP_ALGO(INTERP_ALGO_VAL), 1e-9);

    // Close file
    file.close();
        
    return EXIT_SUCCESS;
}