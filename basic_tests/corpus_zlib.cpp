#include <format>
#include <iostream>
#include <map>

#include "lib_zlib.hpp"

void writeHeader(std::ofstream& file) {
    // Write header to file
    file << "System,";
    file << "Timestamp,";
    file << "Corpus File,";
    file << "Compression Level,";
    file << "Trial,";
    file << "Compression Duration (ns),";
    file << "Decompression Duration (ns),";
    file << "Original Size,";
    file << "Compressed Size,";
    file << "Compression Ratio";
    file << std::endl;
}

void writeResults(std::ofstream& file, const CompressionDecompressionResults& results) {
    file << std::format("{},", results.system);
    file << std::format("{},", results.timestamp);
    file << std::format("{},", results.dataName);
    file << std::format("{},", results.compressionLevel);
    file << std::format("{},", results.trial);
    file << std::format("{},", results.compressionDuration.count());
    file << std::format("{},", results.decompressionDuration.count());
    file << std::format("{},", results.originalSize);
    file << std::format("{},", results.compressedSize);
    file << std::format("{:.16f}", static_cast<double>(results.originalSize) / results.compressedSize);
    file << std::endl;
}

void test(std::ofstream& outFile, const std::string& host, const std::vector<char>& original, 
          const std::string& dataName, const int& trials, const int& level) {
    // Collect stats about original, uncompressed data
    CompressionDecompressionResults results{};
    results.system = host;
    results.dataName = dataName;
    results.compressionLevel = level;
    results.originalSize = original.size();

    for (int trial{1}; trial <= trials; ++trial) {
        // Update results for current trial
        results.timestamp = std::chrono::system_clock::now();
        results.trial = trial;

        // Compress data
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Compressing with level " << level << "..." << std::endl;

        CompressionResults compressionResults{timedZlibCompress(original, level)};

        // Collect compression stats
        results.compressedSize = compressionResults.compressedData.size();
        results.compressionDuration = compressionResults.duration;
        results.compressionRatio = static_cast<double>(results.originalSize) / results.compressedSize;

        // Decompress data
        std::cout << "(" << std::chrono::system_clock::now() << ") ";
        std::cout << "Decompressing data..." << std::endl;

        DecompressionResults decompressionResults{timedZlibDecompress<char>(compressionResults.compressedData, results.originalSize)};

        // Check that decompressed data matches original data
        if (original != decompressionResults.decompressedData) {
            throw std::runtime_error("testCompressionDecompression: decompressed data does not match original data");
        }

        // Collection decompression stats
        results.decompressionDuration = decompressionResults.duration;

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

    // Determine host
    std::string host{getHost()};

    // Read files into vectors
    std::map<std::string, std::vector<char>> fileData{};
    for (const std::string& filepath : CORPUS) {
        fileData[filepath] = fileToVector<char>(filepath);
    }

    // Open file for writing
    std::string filename{std::format("corpus-zlib-l={}.csv", LEVEL)};
    std::ofstream file{filename};

    // Write header to file
    writeHeader(file);

    // Test compression and decompression for each file in corpus
    for (const std::string& filepath : CORPUS) {
        test(file, host, fileData[filepath], filepath, TRIALS, LEVEL);
    }

    // Close file
    file.close();

    return EXIT_SUCCESS;
}