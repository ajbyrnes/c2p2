#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/root.hpp"

int main(int argc, char* argv[]) {
    std::string treename = "CollectionTree";
    std::string branchname = "AnalysisJetsAuxDyn.pt";

    // Read treename, branchname, maxBytes from command line arguments

    try {
        std::vector<float> data = readVectorFloatBranchFromRootFile(
            "../13G-data/DAOD_PHYSLITE.37019878._000001.pool.root.1",   // Path to file
            "CollectionTree",               // Name of tree
            "AnalysisJetsAuxDyn.pt",        // Name of branch
            1024 * 1024 * 1024              // Maximum bytes to read
        );

        writeDecompressedDataToRootFile(
            "test-output.root",
            "TreeName",
            {
                {
                    data,                     // Decompressed data
                    "Compressor",         // Compressor name
                    "DataName",           // Data name
                    "OgDataFileName"      // Original data file name
                }
            }
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}