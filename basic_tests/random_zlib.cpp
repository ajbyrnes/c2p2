#include <chrono>
#include <fstream>
#include <iostream>

#include "lib_zlib.hpp"

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
    file << "Original RMS,";
    file << "Decompressed Mean,";
    file << "Decompressed RMS,";
    file << "Mean CE,";
    file << "RMS CE,";
    file << std::endl;
}

void writeResults(std::ofstream& file, const CompressionDecompressionResults& results) {
    file << std::format("{},", results.system); 
    file << std::format("{},", results.timestamp);  
    file << std::format("{},", results.dataName);
    file << std::format("{},", results.bitsTruncated);
    file << std::format("{},", results.compressionLevel);
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
    file << std::format("{:.16f},", results.rmsCE);
    file << std::endl;
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

void test(std::ofstream& file, const std::string& host, const std::vector<float>& original, 
          const std::string& dataName, const int& trials, const int& level) {
    // Collect original distribution stats
    CompressionDecompressionResults results{};
    results.originalSize = original.size();
    results.system = host;
    results.dataName = dataName;
    results.compressionLevel = level;

    results.originalMean = calculateMean(original);
    results.originalRMS = calculateRMS(original);

    // Print first 10 elements of original data
    std::cout << "Original: ";
    for (int i{0}; i < 10; i++) {
        std::cout << std::format("{:12f}", original[i]) << " ";
    }
    std::cout << std::endl;

    // Test compression and decompression for each number of bits
    for (int bits{0}; bits <= 23; bits++) {
        for (int trial{1}; trial <= trials; trial++) {
            // Update results for current trial
            results.timestamp = std::chrono::system_clock::now();
            results.bitsTruncated = bits;
            results.trial = trial;

            // Compress data
            std::cout << "(" << std::chrono::system_clock::now() << ") ";
            std::cout << "Compressing at level " << level << " with " << bits << "-bit truncation" << std::endl;

            // Collect compression stats
            CompressionResults compResult{timedZlibTruncateCompress(original, bits, level)};
            results.compressedSize = compResult.compressedData.size();
            results.compressionDuration = compResult.duration;

            // Decompress data
            std::cout << "(" << std::chrono::system_clock::now() << ") ";
            std::cout << "Decompressing..." << std::endl;

            DecompressionResults decompResult{timedZlibDecompress<float>(compResult.compressedData, results.originalSize)};

            // Print first 10 elements of decompressed data
            std::cout << "Decompressed: ";
            for (int i{0}; i < 10; i++) {
                std::cout << std::format("{:12f}", decompResult.decompressedData[i]) << " ";
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
}

int main(int argc, char* argv[]) {
    // Process args
    int MODE{0};
    int LEVEL{0};
    int TRIALS{0};
    int DATASIZE{0};

    if (argc != 5) {
        // Usage
        std::cout << "Usage: " << argv[0] << " <level> <dataSize> <mode> <numTrials>" << std::endl;
        std::cout << "<level>: Compression level, 0 - 9" << std::endl;
        std::cout << "<dataSize>: Size of data to compress in MB" << std::endl;
        std::cout << "<mode>:" << std::endl;
        std::cout << "\t1: Uniform Random Data" << std::endl;    
        std::cout << "\t2: Normal Random Data" << std::endl;
        std::cout << "\t3: Sorted Uniform Random Data" << std::endl;
        std::cout << "\t4: Sorted Normal Random Data" << std::endl;
        std::cout << "<numTrials>: Number of times to perform test" << std::endl;
        return EXIT_FAILURE;
    }
    else {
        LEVEL = std::stoi(argv[1]);
        DATASIZE = std::stoi(argv[2]) * MB / sizeof(float);
        MODE = std::stoi(argv[3]);
        TRIALS = std::stoi(argv[4]);
    }

    // Uniform distribution
    float min{0.0};
    float max{32.0};
    std::string uniformName{std::format("uniform_min={}_max={}", min, max)};
    std::string uniformSortedName{std::format("uniform_sorted_min={}_max={}", min, max)};
    std::uniform_real_distribution<float> uniformDistribution(min, max);

    // Normal distribution
    float muNorm{16};
    float sigmaNorm{1.31};
    std::string normalName{std::format("normal_mu={}_sigma={}", muNorm, sigmaNorm)};
    std::string normalSortedName{std::format("normal_sorted_mu={}_sigma={}", muNorm, sigmaNorm)};
    std::normal_distribution<float> normalDistribution(muNorm, sigmaNorm);

    // Determine host
    std::string host{getHost()};

    // Open file for writing
    std::string filename{std::format("random-zlib-d={}-l={}.csv", MODE, LEVEL)};
    std::ofstream file{filename};
    writeHeader(file);

    // Test distributions
    std::vector<float> testData{};
    std::string dataName{};

    std::cout << "(" << std::chrono::system_clock::now() << ") ";
    std::cout << "Generating vector with " << DATASIZE << " floats (" << (DATASIZE * sizeof(float)) / MB << " MB)..." << std::endl;

    int seed{12345};

    switch(MODE) {
        case 1:
            testData = generateRandomTestData(uniformDistribution, seed, DATASIZE, false);
            dataName = uniformName;
            break;
        case 2:
            testData = generateRandomTestData(normalDistribution, seed, DATASIZE, false);
            dataName = normalName;
            break;
        case 3:
            testData = generateRandomTestData(uniformDistribution, seed, DATASIZE, true);
            dataName = uniformSortedName;
            break;
        case 4:
            testData = generateRandomTestData(normalDistribution, seed, DATASIZE, true);
            dataName = normalSortedName;
            break;
        default:
            std::cout << "Invalid distribution" << std::endl;
            return 1;
    }

    test(file, host, testData, dataName, TRIALS, LEVEL);

    // Close file
    file.close();
        
    return EXIT_SUCCESS;
}