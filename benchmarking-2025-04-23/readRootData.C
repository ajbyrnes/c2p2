#include <iostream>
#include <string>
#include <vector>

#include <TFile.h>
#include <TTree.h>

void readRootData() {
    std::string filename{"mc_361106.Zee.1largeRjet1lep.root"};
    std::string treename{"mini"};

    // Open ROOT file
    TFile* file = TFile::Open(filename.c_str());
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Load tree from file
    TTree* tree = (TTree*)file->Get(treename.c_str());
    if (!tree) {
        std::cerr << "Error loading tree: " << treename << std::endl;
        file->Close();
        return;
    }

    // Print tree information
    // tree->Print();

    // Print branch information
    size_t dataSize{10 * 1'000'000 / sizeof(float)};
    std::vector<float> data{};

    size_t numEntries = tree->GetEntries();

    std::cout << "Number of entries: " << numEntries << std::endl;
    std::cout << "Data size: " << dataSize << std::endl;

    std::vector<float>* entry = nullptr;
    tree->SetBranchAddress("lep_pt", &entry);

    for(int n = 0; n < numEntries; ++n) {
        // Load next entry
        tree->GetEntry(n);
        
        // Print entry
        for (size_t j = 0; j < entry->size(); ++j) {
            data.push_back((*entry)[j]);
        }
    }

    // Print first 10 data values
    std::cout << "First 10 data values: ";
    for (size_t i = 0; i < 10 && i < data.size(); ++i) {
        std::cout << data[i] << " ";
    }
    std::cout << std::endl;

    // Print size of data
    std::cout << "Data size: " << data.size() << " floats = " << data.size() * sizeof(float) << " bytes" << std::endl;
}