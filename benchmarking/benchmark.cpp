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
    // std::cerr << "  doSZZlib: " << params.doSZZlib << std::endl;

    std::cerr << "  iterations: " << params.iterations << std::endl;
    std::cerr << "  precision: " << params.precision << std::endl;

    std::cerr << "  dataMB: " << params.dataMB << std::endl;
    std::cerr << "  dataName: " << params.dataName << std::endl;

    // If data is from root file
    std::cerr << "  sourceFile: " << params.sourceFile << std::endl;
    std::cerr << "  treeName: " << params.treeName << std::endl;
    std::cerr << "  branchName: " << params.branchName << std::endl;

    // If data is generated from normal distribution
    std::cerr << "  seed: " << params.seed << std::endl;
    std::cerr << "  mean: " << params.mean << std::endl;
    std::cerr << "  stddev: " << params.stddev << std::endl;

    std::cerr << "  trunkCompressionLevel: " << params.trunkCompressionLevel << std::endl;
    std::cerr << "  szErrorBoundMode: " << params.szErrorBoundMode << std::endl;
    std::cerr << "  szAlgo: " << params.szAlgo << std::endl;
    std::cerr << "  szInterpAlgo: " << params.szInterpAlgo << std::endl;

    std::cerr << "  host: " << getHost() << std::endl;
    std::cerr << "  timestamp: " << timestamp() << std::endl;
    std::cerr << "  debug: " << params.debug << std::endl;

    std::cerr << std::endl;

    // Get data
    size_t dataSize{static_cast<size_t>(params.dataMB * static_cast<double>(MB)) / sizeof(float)};
    std::vector<float> data{};

    if (params.dataName == "normal") {
        data = generateGaussianRandomData(dataSize, params.mean, params.stddev, params.seed);
    }
    else if (params.dataName == "root") {
        data = readRootFile(0, params.sourceFile, params.treeName, params.branchName, params.debug);
    }
    else {
        throw std::invalid_argument("Unknown data source: " + params.dataName);
        exit(1);
    }

    // Run compression benchmarks
    CompressorBench bench(params);
    bench.run(data);

    // Print report
    std::cout << "\n" << bench.generateReport() << std::endl;
}