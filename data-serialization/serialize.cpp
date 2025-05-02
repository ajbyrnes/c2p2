// Read data from root files into 1D vector of floats
// Then serialize that vector to a file
// This avoids needed to include ROOT headers in the main file

#include <iostream>
#include <string>
#include <vector>

#include <TFile.h>

const std::string filepath = "/home/abyrnes/data/altas/mc_361106.Zee.1largeRjet1lep.root";
const std::vector<std::string> floatBranches = {
    
};

int main() {
    // Step 1: Read data from ROOT file
    std::
}