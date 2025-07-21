#pragma once

#include <string>
#include <vector>

#include "../compressors/Compressor.hpp"

struct Args {
    std::string inputFile = "../data/DAOD_PHYSLITE.37019878._000001.pool.root.1"; // Default input file
    std::string treename = "CollectionTree"; // Default tree name
    std::string branchname = "AnalysisJetsAuxDyn.pt"; // Default branch name
    size_t maxBytes = 1'000'000'000; // Default maximum bytes to read from files (1 GB)

    std::string compressor = "TruncCompressor";
    int iterations = 1; // Default number of iterations for benchmarks
    std::string benchmarkOutputFile = "benchmark-results.csv"; // Default output file for benchmark results

    int mantissaBits = 8; // Default mantissa bits
    int compressionLevel = 9; // Default compression level

    int algo = 0;
    float relErrorBound = 10e-3;
};

const Args parseArgs(int argc, char* argv[]);

std::string getSizeString(size_t numBytes);
std::string getTimestamp(bool filenameSafe = false);
std::string getHost();

std::string timeMessage(const std::string& message);

const std::vector<std::string> dataFiles = {
    "../data/DAOD_PHYSLITE.37019878._000001.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000002.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000003.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000004.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000006.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000007.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000008.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000009.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000011.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000012.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000013.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000014.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000015.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000016.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000017.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000018.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000019.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000020.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000021.pool.root.1",
    "../data/DAOD_PHYSLITE.37019878._000022.pool.root.1"
    "../data/DAOD_PHYSLITE.37019878._000023.pool.root.1"
};

std::vector<float> readVectorFloatBranchFromFile(
    const std::string& filename, 
    const std::string& treename, 
    const std::string& branchname, 
    size_t maxBytes = 1'000'000'000
);

std::vector<float> readVectorFloatBranchFromFiles(
    const std::vector<std::string>& filenames,
    const std::string& treename,
    const std::string& branchname,
    size_t maxBytes = 1'000'000'000
);

void writeDecompressedDataToFile(
    const std::string& filename, 
    const std::string& treename,
    const std::vector<DecompressedData>& data
);  