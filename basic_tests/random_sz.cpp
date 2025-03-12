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
    file << "Distribution,";
    file << "Bits Truncated,";
    file << "Compression Level,";
    file << "Trial,";
    file << "Compression Duration (ns),";
    file << "Decompression Duration (ns),";
    file << "Original Size,";
    file << "Compressed Size,";
    file << "Compression Ratio,";
    file << "Original Mean,";
    file << "Original Mean Error,";
    file << "Original RMS,";
    file << "Original RMS Error,";
    file << "Decompressed Mean,";
    file << "Decompressed Mean Error,";
    file << "Decompressed RMS,";
    file << "Decompressed RMS Error,";
    file << "Mean CE,";
    file << "Mean Error CE,";
    file << "RMS CE,";
    file << "RMS Error CE" << std::endl;
}

void writeResults(std::ofstream& file, const CompressionDecompressionResults& results) {
    // file << std::format("{},", results.system); 
    // file << std::format("{},", results.timestamp);  
    // file << std::format("{},", results.dataName);
    // file << std::format("{},", results.bitsTruncated);
    // file << std::format("{},", results.compressionLevel);
    // file << std::format("{},", results.trial);
    // file << std::format("{},", results.compressionDuration.count());
    // file << std::format("{},", results.decompressionDuration.count());
    // file << std::format("{},", results.originalSize);
    // file << std::format("{},", results.compressedSize);
    // file << std::format("{:.16f},", results.compressionRatio);
    // file << std::format("{:.16f},", results.originalMean);
    // file << std::format("{:.16f},", results.originalMeanError);
    // file << std::format("{:.16f},", results.originalRMS);
    // file << std::format("{:.16f},", results.originalRMSError);
    // file << std::format("{:.16f},", results.decompressedMean);
    // file << std::format("{:.16f},", results.decompressedMeanError);
    // file << std::format("{:.16f},", results.decompressedRMS);
    // file << std::format("{:.16f},", results.decompressedRMSError);
    // file << std::format("{:.16f},", results.meanCE);
    // file << std::format("{:.16f},", results.meanErrorCE);
    // file << std::format("{:.16f},", results.rmsCE);
    // file << std::format("{:.16f}", results.rmsErrorCE);
    // file << std::endl;
}

template <typename Distribution>
std::vector<float> generateRandomTestData(const Distribution& distribution, const int& seed, const int& dataSize, const bool& sorted) {
    std::vector<float> data{generateRandomData(distribution, seed, dataSize)};

    if (sorted) {
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Sorting vector..." << std::endl;
        std::sort(data.begin(), data.end());
    }

    return data;
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

    TH1F* h{vectorToHistogram(original, dataName)};
    results.originalMean = h->GetMean();
    results.originalMeanError = h->GetMeanError();
    results.originalRMS = h->GetRMS();
    results.originalRMSError = h->GetRMSError();
    delete h;

    // Test compression and decompression for each number of bits
    for (int trial{1}; trial <= trials; trial++) {
        // Update results for current trial
        results.timestamp = std::chrono::system_clock::now();
        results.trial = trial;

        // Compress data
        // std::cout << "(" << std::chrono::system_clock::now() << ") ";
        // std::cout << "Compressing..." << std::endl;
        
        // Collect compression stats
        CompressionResults compResult{timedSzCompress(original, errorBoundMode, algo, interpAlgo, absErrorBound)};
        results.compressedSize = compResult.compressedData.size();
        results.compressionDuration = compResult.duration;

        // Decompress data
        // std::cout << "(" << std::chrono::system_clock::now() << ") ";
        // std::cout << "Decompressing..." << std::endl;

        // DecompressionResults decompResult{timedZlibDecompress<float>(compResult.compressedData, results.originalSize)};

        // Collect decompressed distribution stats
        // results.decompressionDuration = decompResult.duration;

        // TH1F* h{vectorToHistogram(decompResult.decompressedData, std::format("hist_{}_{}", dataName, bits))};

        // results.decompressedMean = h->GetMean();
        // results.decompressedMeanError = h->GetMeanError();
        // results.decompressedRMS = h->GetRMS();
        // results.decompressedRMSError = h->GetRMSError();
        
        // results.meanCE = std::abs(results.originalMean - results.decompressedMean);
        // results.meanErrorCE = std::abs(results.originalMeanError - results.decompressedMeanError);
        // results.rmsCE = std::abs(results.originalRMS - results.decompressedRMS);
        // results.rmsErrorCE = std::abs(results.originalRMSError - results.decompressedRMSError);
        
        // delete h;

        // Calculate compression ratio
        // results.compressionRatio = static_cast<double>(results.originalSize) / results.compressedSize;

        // Write results to file
        // writeResults(file, results);
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

    if (argc != 7) {
        // Usage
        std::cout << "Usage: " << argv[0] << " <eb> <algo> <interpAlgo> <dataSize> <mode> <numTrials>" << std::endl;
        std::cout << "<eb>: Error bound mode" << std::endl;
        std::cout << "<algo>: Compression algorithm" << std::endl;
        std::cout << "<interpAlgo>: Interpolation algorithm" << std::endl;
        std::cout << "<datasize>: Size of data to compress in MB" << std::endl;
        std::cout << "<mode>:" << std::endl;
        std::cout << "\t1: Uniform Random Data" << std::endl;    
        std::cout << "\t2: Normal Random Data" << std::endl;
        std::cout << "\t3: Sorted Uniform Random Data" << std::endl;
        std::cout << "\t4: Sorted Normal Random Data" << std::endl;
        std::cout << "<numTrials>: Number of times to perform test" << std::endl;
        return EXIT_FAILURE;
    }
    else {
        EB_VAL = SZ3::EB(std::stoi(argv[1]));
        ALGO_VAL = SZ3::ALGO(std::stoi(argv[2]));
        INTERP_ALGO_VAL = SZ3::INTERP_ALGO(std::stoi(argv[3]));
        DATASIZE = std::stoi(argv[4]) * MB / sizeof(float);
        MODE = std::stoi(argv[5]);
        TRIALS = std::stoi(argv[6]);
    }

    // Determine host
    std::string host{getHost()};

    // Uniform distribution
    // float min{0.0};
    // float max{32.0};
    // std::string uniformName{std::format("uniform_min={}_max={}", min, max)};
    // std::string uniformSortedName{std::format("uniform_sorted_min={}_max={}", min, max)};
    // std::uniform_real_distribution<float> uniformDistribution(min, max);

    // Normal distribution
    float muNorm{16};
    // float sigmaNorm{1.31};
    // std::string normalName{std::format("normal_mu={}_sigma={}", muNorm, sigmaNorm)};
    // std::string normalSortedName{std::format("normal_sorted_mu={}_sigma={}", muNorm, sigmaNorm)};
    // std::normal_distribution<float> normalDistribution(muNorm, sigmaNorm);

    // Open file for writing
    // std::string filename{std::format("random-zlib-d={}-l={}.csv", MODE, LEVEL)};
    // std::ofstream file{filename};
    // writeHeader(file);

    // Test distributions
    // std::vector<float> testData{};
    // std::string dataName{};

    // std::cout << "(" << std::chrono::system_clock::now() << ") ";
    // std::cout << "Generating vector with " << DATASIZE << " floats (" << (DATASIZE * sizeof(float)) / MB << " MB)..." << std::endl;

    int seed{12345};

    // switch(MODE) {
    //     case 1:
    //         testData = generateRandomTestData(uniformDistribution, seed, DATASIZE, false);
    //         dataName = uniformName;
    //         break;
    //     case 2:
    //         testData = generateRandomTestData(normalDistribution, seed, DATASIZE, false);
    //         dataName = normalName;
    //         break;
    //     case 3:
    //         testData = generateRandomTestData(uniformDistribution, seed, DATASIZE, true);
    //         dataName = uniformSortedName;
    //         break;
    //     case 4:
    //         testData = generateRandomTestData(normalDistribution, seed, DATASIZE, true);
    //         dataName = normalSortedName;
    //         break;
    //     default:
    //         std::cout << "Invalid distribution" << std::endl;
    //         return 1;
    // }

    // test(file, host, testData, dataName, TRIALS, LEVEL);

    // Close file
    // file.close();
        
    return EXIT_SUCCESS;
}