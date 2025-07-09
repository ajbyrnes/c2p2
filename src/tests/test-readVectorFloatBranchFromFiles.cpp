#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/utils.hpp"

int main(int argc, char* argv[]) {
    std::string treename = "CollectionTree";
    std::string branchname = "AnalysisJetsAuxDyn.pt";
    size_t maxBytes = 1'000'000'000; // 1 GB

    // Read treename, branchname, maxBytes from command line arguments

    std::vector<float> data;

    try {
        auto args = parseArgs(argc, argv);
        data = readVectorFloatBranchFromFiles(dataFiles, treename, branchname, 1'000'000'000);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}