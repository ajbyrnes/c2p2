
/**
 * @file cli.hpp
 * @brief Declarations for command-line argument parsing and argument struct.
 */
#pragma once

#include <string>

/**
 * @struct Args
 * @brief Structure to hold parsed command-line arguments and their default values.
 *
 * Members:
 *   inputFile            Input file path
 *   treename             Tree name in the input file
 *   branchname           Branch name in the tree
 *   maxBytes             Maximum bytes to read from files
 *   compressor           Compressor name
 *   iterations           Number of benchmark iterations
 *   benchmarkOutputFile  Output file for benchmark results
 *   mantissaBits         Mantissa bits for compression
 *   compressionLevel     Compression level
 *   algo                 Algorithm selection
 *   relErrorBound        Relative error bound
 */
struct Args {
    std::string inputFile = "../data/DAOD_PHYSLITE.37019878._000001.pool.root.1"; // Default input file
    std::string treename = "CollectionTree"; // Default tree name
    std::string branchname = "AnalysisJetsAuxDyn.pt"; // Default branch name
    size_t maxBytes = 1024 * 1024 * 1024; // Default maximum bytes to read from files (1 GB)

    std::string compressor = "TruncCompressor";
    int iterations = 1; // Default number of iterations for benchmarks
    std::string benchmarkOutputFile = "benchmark-results.csv"; // Default output file for benchmark results

    int mantissaBits = 8; // Default mantissa bits
    int compressionLevel = 9; // Default compression level

    int algo = 0;
    float relErrorBound = 10e-3;
};

/**
 * @brief Parse command-line arguments into an Args struct.
 * @param argc Number of command-line arguments.
 * @param argv Array of argument strings.
 * @return Args struct populated with parsed values.
 */
const Args parseArgs(int argc, char* argv[]);