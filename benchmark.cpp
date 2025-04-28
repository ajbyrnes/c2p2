#include <iostream>
#include <fstream>
#include <format>
#include <vector>

#include "lib/CompressorBench.hpp"
#include "lib/utils.hpp"

std::ofstream& openCSV(std::string filename) {
    std::ofstream& file{*(new std::ofstream(filename))};
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Write header
    file << "Compresser,";
    file << "Iterations,";
    file << "DataName,";
    file << "Precision,";
    file << "CompressionLevel,";
    file << "ErrorBoundMode,";
    file << "Algo,";
    file << "InterpAlgo,";
    file << "CompressionTimeMS,";
    file << "DecompressionTimeMS,";
    file << "CompressionRatio,";
    file << "CompressedDataSize,";
    file << "OriginalDataSize,";
    file << "AvgRelativeError";
    file << std::endl;
    return file;
}

int main(int argc, char* argv[]) {
    // Set parameters
    BenchmarkParams params{parseArguments(argc, argv)};

    // Print parameters
    std::cerr << "Parameters:" << std::endl;
    std::cerr << "  doTrunk: " << params.doTrunk << std::endl;
    std::cerr << "  doSZ: " << params.doSZ << std::endl;
    std::cerr << "  iterations: " << params.iterations << std::endl;
    std::cerr << "  precision: " << params.precision << std::endl;
    std::cerr << "  debug: " << params.debug << std::endl;
    std::cerr << "  dataMB: " << params.dataMB << std::endl;
    std::cerr << "  dataSource: " << params.dataName << std::endl;
    std::cerr << "  branchName: " << params.branchName << std::endl;
    std::cerr << "  seed: " << params.seed << std::endl;
    std::cerr << "  mean: " << params.mean << std::endl;
    std::cerr << "  stddev: " << params.stddev << std::endl;
    std::cerr << "  trunkCompressionLevel: " << params.trunkCompressionLevel << std::endl;
    std::cerr << "  szErrorBoundMode: " << params.szErrorBoundMode << std::endl;
    std::cerr << "  szAlgo: " << params.szAlgo << std::endl;
    std::cerr << "  szInterpAlgo: " << params.szInterpAlgo << std::endl;
    std::cerr << "  host: " << getHost() << std::endl;
    std::cerr << "  timestamp: " << timestamp() << std::endl;
    std::cerr << std::endl;

    // Open file for writing
    // std::string filename{
    //     std::format("SZ3Sweep-{}-{}-{}.csv",
    //                 params.dataName, getHost(), timestamp())
    // };

    // std::ofstream& file{openCSV(filename)};

    // Get data
    size_t dataSize{static_cast<size_t>(params.dataMB * static_cast<double>(MB)) / sizeof(float)};
    std::vector<float> data{};

    if (params.dataName == "normal") {
        data = generateGaussianRandomData(dataSize, params.mean, params.stddev, params.seed);
    }
    else {
        data = readRootFile(0, params.sourceFile, params.dataName, params.branchName);
    }

    // Run compression benchmarks
    CompressorBench bench(params);
    bench.run(data);

    // Print results
    for (int compressor{CompressorBench::TRUNK}; compressor <= CompressorBench::SZ; ++compressor) {
        if ((compressor == CompressorBench::TRUNK && !params.doTrunk) || (compressor == CompressorBench::SZ && !params.doSZ)) {
            continue;
        }

        CompressorBench::COMPRESSOR compressorEnum{static_cast<CompressorBench::COMPRESSOR>(compressor)};
        std::cerr << "[DEBUG benchmark] Compressor: " << (compressor == CompressorBench::TRUNK ? "Trunk" : "SZ") << std::endl;
        std::cerr << "[DEBUG benchmark] Compression time: " << bench.getCompressionTime(compressorEnum) << " ms" << std::endl;
        std::cerr << "[DEBUG benchmark] Decompression time: " << bench.getDecompressionTime(compressorEnum) << " ms" << std::endl;
        std::cerr << "[DEBUG benchmark] Compression ratio: " << bench.getCompressionRatio(compressorEnum) << std::endl;
        std::cerr << "[DEBUG benchmark] Compressed data size: " << bench.getCompressedDataSize(compressorEnum) / KB << " KB" << std::endl;
        std::cerr << "[DEBUG benchmark] Original data size: " << bench.getOriginalDataSize() / KB << " KB" << std::endl;
    }

    // Print report
    std::cout << bench.generateReport();

    // Close file
    // file.close();
}