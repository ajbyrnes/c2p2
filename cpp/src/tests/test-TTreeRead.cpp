#include <format>

/**
 * @file root.cpp
 * @brief Utilities for reading from and writing to ROOT files using TTree and branches.
 */
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include <unistd.h>

#include <TFile.h>
#include <TError.h>
#include <TTree.h>

#include "../utils/utils.hpp"
#include "../utils/root.hpp"

int main() {
    // Open ROOT file
    std::string filename = "/home/ajbyrnes/data/data16_13TeV/DAOD_PHYSLITE.37019878/DAOD_PHYSLITE.37019878._000001.pool.root.1";
    std::string treename = "CollectionTree";
    std::string branchname = "AnalysisJetsAuxDyn.pt";
    
    try {
        std::vector<std::vector<float>> data = readVectorFloatBranch(filename, treename, branchname, 1e9);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}