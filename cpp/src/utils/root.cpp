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

#include <TTreeReader.h>
#include <TTreeReaderValue.h>
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

    // Use TTreeReader for efficient reading
    TTreeReader reader(treename.c_str(), file);
    TTreeReaderValue<std::vector<float>> branch(reader, branchname.c_str());

    std::vector<std::vector<float>> entries;
    Long64_t bytesRead{0};
    Long64_t totalValues{0};

    std::cout << timeMessage(std::format("Reading entries from branch '{}' in file '{}'", branchname, filename));
    std::cout << std::endl;

    while (reader.Next()) {
        if (bytesRead + (branch->size() * sizeof(float)) > maxBytes) {
            std::cout << timeMessage(std::format(
                "Reached maxBytes limit ({} bytes), stopping read after {} entries", 
                getSizeString(maxBytes), entries.size()
            ));
            std::cout << std::endl;
            break;
        }
        entries.push_back(*branch);
        totalValues += static_cast<Long64_t>(branch->size());
        bytesRead += static_cast<Long64_t>(branch->size() * sizeof(float));
    }

    file->Close();
    delete file;

    return entries;
}