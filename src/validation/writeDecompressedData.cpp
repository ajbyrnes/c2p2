#include <format>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "../utils/utils.hpp"
#include "../compressors/TruncCompressor.hpp"
#include "../compressors/SZ3Compressor.hpp"

int main(int argc, char* argv[]) {
    // Parse args
    Args args{parseArgs(argc, argv)};

    // Read treename, branchname, maxBytes from command line arguments
    std::vector<float> data = readVectorFloatBranchFromFiles(
        dataFiles, args.treename, args.branchname, args.maxBytes
    );

    // Compress and decompress data with TruncCompressor
    std::vector<int> compressionLevels{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> mantissaBits{16, 12, 8};

    bool firstWrite = true;

    for (int bits : mantissaBits) {
        for (int level : compressionLevels) {
            std::cout << std::format("[{}] Compressing data with TruncCompressor (level: {}, mantissa bits: {})", 
                                     getTimestamp(), level, bits) << std::endl;
            TruncCompressor truncCompressor(level, bits);
            CompressedData truncCompressedData = truncCompressor.compress({data, args.branchname});
            std::cout << std::format("[{}] Compressed data with TruncCompressor", getTimestamp()) << std::endl;
            std::vector<float> truncDecompressedData = truncCompressor.decompress(truncCompressedData);
            std::cout << std::format("[{}] Decompressed data with TruncCompressor", getTimestamp()) << std::endl;

            // Append to existing file
            writeFloatVectorToFile("decompressed.root", truncDecompressedData, truncCompressor.toString(), args.branchname, false);
        }
    }

    std::vector<int> algorithms{0, 1, 2, 3};
    std::vector<float> relErrorBounds{5e-3, 5e-4, 5e-5};

    for (int algo : algorithms) {
        for (float relError : relErrorBounds) {
            std::cout << std::format("[{}] Compressing data with SZ3Compressor (algorithm: {}, relative error bound: {})", 
                                     getTimestamp(), algo, relError) << std::endl;
            SZ3Compressor sz3Compressor(relError, algo);

            try {
                CompressedData sz3CompressedData = sz3Compressor.compress({data, args.branchname});
                std::cout << std::format("[{}] Compressed data with SZ3Compressor", getTimestamp()) << std::endl;
                std::vector<float> sz3DecompressedData = sz3Compressor.decompress(sz3CompressedData);
                std::cout << std::format("[{}] Decompressed data with SZ3Compressor", getTimestamp()) << std::endl;

                writeFloatVectorToFile("decompressed.root", sz3DecompressedData, sz3Compressor.toString(), args.branchname, false);
            } catch (const std::exception& e) {
                std::cerr << std::format("[{}] Error during compression/decompression with SZ3Compressor: {}", 
                                         getTimestamp(), e.what()) << std::endl;
            }
        }
    }

    return 0;
}