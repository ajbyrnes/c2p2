#include <format>
#include <iostream>

#include "lib_test.hpp"
#include "lib_zlib.hpp"

void writeHeader(std::ofstream& file) {
    // Write header to file
    file << "File,";
    file << "Compression Level,";
    file << "Trial,";
    file << "Compression Duration (ns),";
    file << "Decompression Duration (ns),";
    file << "Original Size,";
    file << "Compressed Size,";
    file << "Compression Ratio";
}

void writeResults(std::ofstream& file, const CompressionDecompressionResults& results) {
    std::cout << std::format("{},", results.dataName);
    std::cout << std::format("{},", results.compressionLevel);
    std::cout << std::format("{},", results.trial);
    std::cout << std::format("{},", results.compressionDuration.count());
    std::cout << std::format("{},", results.decompressionDuration.count());
    std::cout << std::format("{},", results.originalSize);
    std::cout << std::format("{},", results.compressedSize);
    std::cout << std::format("{:.16f}", static_cast<double>(results.originalSize) / results.compressedSize);
    std::cout << std::endl;
}

void testCompressionDecompression(std::ofstream& outFile, const std::string& dataFile, const int& trials, const int& level) {
    // Read file data
    std::cout << "(" << std::chrono::system_clock::now() << ") ";
    std::cout << "Reading file: " << dataFile << std::endl;

    std::vector<uint8_t> original{charFileToVector(dataFile)};

    // Collect stats about original, uncompressed data
    CompressionDecompressionResults results{};
    results.dataName = dataFile;
    results.compressionLevel = level;
    results.originalSize = original.size();

    for (int trial{1}; trial <= trials; ++trial) {
        // Update results for current trial
        results.trial = trial;

        // Compress data
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Compressing data..." << std::endl;

        CompressionResults compressionResults{timedZlibCompress(original, level)};

        // Collect compression stats
        results.compressedSize = compressionResults.compressedData.size();
        results.compressionDuration = compressionResults.duration;
        results.compressionRatio = static_cast<double>(results.originalSize) / results.compressedSize;

        // Decompress data
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Decompressing data..." << std::endl;

        DecompressionResults decompressionResults{timedZlibDecompress<uint8_t>(compressionResults.compressedData, results.originalSize)};

        // Collection decompression stats
        results.decompressionDuration = decompressionResults.duration;

        // Check that decompressed data matches original data
        if (original != decompressionResults.decompressedData) {
            throw std::runtime_error("testCompressionDecompression: decompressed data does not match original data");
        }

        // Write results
        writeResults(outFile, results);
    }
}

int main(int argc, char* argv[]) {
    int LEVEL{0};
    int TRIALS{0};

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <level> <numTrials>" << std::endl;
        std::cerr << "Compression level: 0 - 9" << std::endl;
        return EXIT_FAILURE;
    }
    else {
        LEVEL = std::stoi(argv[1]);
        TRIALS = std::stoi(argv[2]);
    }

    // Open file for writing
    std::string filename{"corpus-zlib.csv"};
    std::ofstream file{filename};

    // Test compression and decompression for each file in corpus
    for (const std::string& filepath : CORPUS) {
        testCompressionDecompression(file, filepath, TRIALS, LEVEL);
    }

    // Close file
    file.close();

    return EXIT_SUCCESS;
}