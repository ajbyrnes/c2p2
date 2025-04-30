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

    // Branches to read
    std::vector<std::string> branches{
        "lep_pt",
        "jet_pt",
        "largeRjet_pt",
        "lep_z0",
        "lep_ptcone30",
        "lep_trackd0pvunbiased",
        "lep_tracksigd0pvunbiased",
        "lep_pt_syst",
        "jet_pt_syst",
        "largeRjet_truthMatched"
    };
    
    std::vector<std::vector<float>> data(branches.size(), std::vector<float>{});

    // Open file for writing
    std::ofstream file{"rootData.csv"};
    if (!file.is_open()) {
        std::cerr << std::format("[ERROR] Could not open file: rootData.csv") << std::endl;
        return 1;
    }

    // Write header
    for (const std::string& branch : branches) {
        file << branch << ",";
    }
    file << std::endl;

    for (int b{0}; b < branches.size(); b++) {
        // Read data
        data[b] = readRootFile(params.dataMB, params.sourceFile, "mini", branches[b]);
        if (data[b].empty()) {
            std::cerr << std::format("[ERROR] No data found in branch {}", branches[b]) << std::endl;
            continue;
        }
    }

    // Write data
    for (int i{0}; i < data[0].size(); ++i) {
        for (int b{0}; b < branches.size(); b++) {
            file << data[b][i] << ",";
        }
        file << std::endl;
    }

    // Close file
    file.close();
}