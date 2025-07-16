#include <format>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/utils.hpp"
#include "../utils/datagen.hpp"

int main(int argc, char* argv[]) {
    std::string treename = "CollectionTree";
    std::string branchname = "AnalysisJetsAuxDyn.pt";

    // Read treename, branchname, maxBytes from command line arguments

    try {
        std::vector<float> data1 = readVectorFloatBranchFromFile(
            dataFiles[0], "CollectionTree", "AnalysisJetsAuxDyn.pt"
        );

        std::vector<float> data2 = constant1D(1000, 42.0f);

        writeFloatVectorToFile("test-output.root", data1, "OutputTree", "data1", false);
        writeFloatVectorToFile("test-output.root", data2, "OutputTree", "data2", false);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}