
#include <chrono>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include "../utils/utils.hpp"
#include "../utils/root.hpp"

int main() {
    // Open ROOT file
    std::string filename = "/home/ajbyrnes/data/data16_13TeV/DAOD_PHYSLITE.37019878/DAOD_PHYSLITE.37019878._000009.pool.root.1";
    std::string treename = "CollectionTree";
    std::string branchname = "AnalysisJetsAuxDyn.phi";
    
    try {
        auto start = std::chrono::steady_clock::now();
        std::vector<std::vector<float>> data = readVectorFloatBranch(filename, treename, branchname, 1e9);
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        std::cout << "Read " << data.size() << " entries from branch '" << branchname << "' in " << elapsed_seconds.count() << " seconds." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}