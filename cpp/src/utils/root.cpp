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

#include "root.hpp"
#include "utils.hpp"

/**
 * @brief Reads all float values from a specified branch in a ROOT file.
 *
 * Assumes the branch contains std::vector<float> data. Only guaranteed to work for branches:
 *   - AnalysisJetsAuxDyn.pt
 *   - AnalysisJetsAuxDyn.eta
 *   - AnalysisJetsAuxDyn.phi
 *
 * @param filename    Path to the ROOT file.
 * @param treename    Name of the tree in the file.
 * @param branchname  Name of the branch to read.
 * @param maxBytes    Maximum number of bytes to read (stops early if exceeded).
 * @return std::vector<float> containing all float values from the branch.
 * @throws std::runtime_error if file or tree cannot be opened.
 */
std::vector<std::vector<float>> readVectorFloatBranch(
    const std::string& filename, 
    const std::string& treename, 
    const std::string& branchname, 
    size_t maxBytes
) 
{
    // Suppress warnings like the following:
    // Warning in <TClass::Init>: no dictionary for class xAOD::EventInfo_v1 is available
    // We're just reading vector<float> branches here, so we don't actually need the info for any of these ATLAS classes
    gErrorIgnoreLevel = kError;

    // Open the ROOT file
    TFile* file = nullptr; 
    
    try {
        file = TFile::Open(filename.c_str(), "READ");
    } catch (const std::exception& e) {
        std::cerr << "Error opening file: " << e.what() << std::endl;
        exit(1);
    }

    if (!file || file->IsZombie()) {
        throw std::runtime_error("Failed to open file");
        exit(1);
    }

    // Get the tree from the file
    TTree* tree{dynamic_cast<TTree*>(file->Get(treename.c_str()))};
    if (!tree) {
        throw std::runtime_error("Failed to get tree");
        exit(2);
    }

    // Enable only the desired branch
    tree->SetBranchStatus("*", 0);
    tree->SetBranchStatus(branchname.c_str(), 1);

    // Set the branch address to read the vector<float> entries
    std::vector<float>* entryData = nullptr;

    tree->SetBranchAddress(branchname.c_str(), &entryData);    
    Long64_t nEntries = tree->GetEntries();

    std::cout << timeMessage(std::format("Reading {} entries from branch '{}' in file '{}'", nEntries, branchname, filename));
    std::cout << std::endl;

    std::vector<std::vector<float>> entries;
    Long64_t bytesRead{0};
    Long64_t totalValues{0};

    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        // Check if data in entry would push us over maxBytes
        if (bytesRead + (entryData->size() * sizeof(float)) > maxBytes) {
            std::cout << timeMessage(std::format(
                "Reached maxBytes limit ({} bytes), stopping read after {} entries", 
                getSizeString(maxBytes), i - 1
            ));
            std::cout << std::endl;
            break;
        }

        entries.push_back(*entryData);
        totalValues += static_cast<Long64_t>(entryData->size());
        bytesRead += static_cast<Long64_t>(entryData->size() * sizeof(float));
    }

    file->Close();
    delete file;
    delete tree;
    delete entryData;

    // Size can be reported in GB, MB, KB, or bytes
    int numBytes = totalValues * sizeof(float);
    std::cout << timeMessage(
        std::format(
            "Read {} values ({}) from {} entries from branch '{}'", 
            totalValues, getSizeString(bytesRead), entries.size(), branchname
        )
    );
    std::cout << std::endl;

    return entries;
}

void writeVectorFloatBranch(
    const std::string filename,
    const std::string treename,
    const std::string branchname,
    const std::vector<std::vector<float>>& data
)
{
    // Create or open the ROOT file
    TFile* file{TFile::Open(filename.c_str(), "UPDATE")};
    if (!file || file->IsZombie()) {
        throw std::runtime_error("Failed to open file for writing");
        exit(1);
    }

    // Create or get the tree from the file
    TTree* tree{dynamic_cast<TTree*>(file->Get(treename.c_str()))};
    bool newTree{false};
    if (!tree) {
        newTree = true;
        tree = new TTree(treename.c_str(), "A tree with float vector branch");
    }

    // Create the branch
    // If its the first branch in the tree we don't need to iterate over existing entries
    std::vector<float>* branchData{nullptr};

    if (!newTree) {
        tree->Branch(branchname.c_str(), &branchData);
        for (int i = 0; i < data.size(); ++i) {
            branchData = &data[i];
            tree->Fill();
        }

    }
    else {
        // Create branch
        TBranch* branch = tree->Branch(branchname.c_str(), &branchData);

        // Disable other branches
        tree->SetBranchStatus("*", 0);
        tree->SetBranchStatus(branchname.c_str(), 1);
        
        for (int i = 0; i < data.size(); ++i) {
            tree->GetEntry(i);
            branchData = &data[i];
            branch->Fill();
        }
    }

    tree->Write();
    file->Close();
    delete file;
    delete tree;
    delete branchData;
}