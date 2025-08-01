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
std::vector<float> readVectorFloatBranchFromRootFile(
    const std::string& filename, 
    const std::string& treename, 
    const std::string& branchname, 
    size_t maxBytes) 
{
    // Suppress warnings like the following:
    // Warning in <TClass::Init>: no dictionary for class xAOD::EventInfo_v1 is available
    // We're just reading vector<float> branches here, so we don't actually need the info for any of these ATLAS classes
    gErrorIgnoreLevel = kError;

    std::vector<float> flatData{};
    std::vector<float>* entryData = nullptr;

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
    tree->SetBranchAddress(branchname.c_str(), &entryData);    
    Long64_t nEntries = tree->GetEntries();

    std::cout << timeMessage(std::format("Reading {} entries from branch '{}' in file '{}'", nEntries, branchname, filename));
    std::cout << std::endl;

    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        // Push entries into flatData
        for (const auto& value : *entryData) {
            flatData.push_back(value);
        }

        // Check if we exceed the maximum bytes limit
        if (flatData.size() * sizeof(float) >= maxBytes) {
            std::cout << timeMessage(std::format("Reached maximum size of {} bytes, stopping read at entry {}", maxBytes, i));
            std::cout << std::endl;
            break;
        }
    }

    file->Close();
    delete file;

    // Size can be reported in GB, MB, KB, or bytes
    int numBytes = flatData.size() * sizeof(float);
    std::cout << timeMessage(
        std::format(
            "Read {} values ({}) from {} entries from branch '{}'", 
            flatData.size(), getSizeString(numBytes), nEntries, branchname
        )
    );
    std::cout << std::endl;

    return flatData;
}

/**
 * @brief Writes decompressed data sets to a ROOT file as branches in a TTree.
 *
 * Each DecompressedData set is written as a separate branch. The tree is created or updated in the file.
 *
 * @param filename Path to the ROOT file to write to.
 * @param treename Name of the tree to create or update.
 * @param data     Vector of DecompressedData sets to write as branches.
 * @throws std::runtime_error if the file cannot be opened for writing.
 */
void writeDecompressedDataToRootFile(
    const std::string& filename, 
    const std::string& treename,
    const std::vector<DecompressedData>& data
)
{
    size_t numDataSets = data.size();
    size_t numEntries = data.front().data.size();

    // Open the ROOT file
    TFile file(filename.c_str(), "UPDATE");
    if (!file.IsOpen()) {
        throw std::runtime_error("Failed to open file for writing");
    }

    // Create TTree
    // Tree name should be dataName_dataFile
    // std::string treename = std::format("{}_{}", data.front().dataName, data.front().ogDataFileName);
    TTree tree(treename.c_str(), "Decompressed Data");

    // We are going to write one branch per set of DecompressedData
    std::vector<float> branchEntries(data.size(), 0);

    // Create branches for each DecompressedData
    for (size_t d = 0; d < numDataSets; ++d) {
        tree.Branch(data[d].compressor.c_str(), &branchEntries[d]);
    }

    // Iterate over the decompressed data and fill the branches
    for (size_t i = 0; i < numEntries; ++i) {
        // Pull the next entries from each set of DecompressedData
        for (size_t d = 0; d < numDataSets; ++d) {
            branchEntries[d] = data[d].data[i];
        }

        // Write to the tree
        tree.Fill();
        if (i % (data.front().data.size() / 10) == 0) {
            std::cout << timeMessage(std::format("Wrote entry {} of {} to tree '{}'", i, data.front().data.size(), treename));
            std::cout << std::endl;
        }
    }

    // Write the tree
    tree.Write();

    // Close the file
    file.Close();

    std::cout << timeMessage(std::format("Wrote {} entries to tree '{}' in file '{}'", data.front().data.size(), treename, filename));
    std::cout << std::endl;
}