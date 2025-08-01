/**
* @file test-readVectorFloatBranchFromFile.cpp
* @brief Test program for reading a vector of floats from a ROOT file branch.
* @details This program reads a vector of floats from a specified branch in a ROOT file
* and prints the first 10 entries. It is designed to be run as a standalone executable
* for testing the `readVectorFloatBranchFromFile` function defined in `utils.hpp`.
**/

#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/root.hpp"

int main(int argc, char* argv[]) {
    std::vector<float> data;
    try {
        data = readVectorFloatBranchFromRootFile(
            "../13G-data/DAOD_PHYSLITE.37019878._000001.pool.root.1",   // Path to file
            "CollectionTree",               // Name of tree
            "AnalysisJetsAuxDyn.pt",        // Name of branch
            1024 * 1024 * 1024              // Maximum bytes to read
        );
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Print the first 10 entries
    std::cout << "First 10 entries:" << std::endl;
    for (size_t i = 0; i < std::min(data.size(), size_t(10)); ++i) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}