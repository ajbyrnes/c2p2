
/**
 * @file root.hpp
 * @brief Declarations for reading and writing data to ROOT files.
 */
#pragma once

#include <vector>
#include "compression.hpp"


/**
 * @brief Reads all float values from a specified branch in a ROOT file.
 * @param filename    Path to the ROOT file.
 * @param treename    Name of the tree in the file.
 * @param branchname  Name of the branch to read.
 * @param maxBytes    Maximum number of bytes to read (default: 1,000,000,000).
 * @return std::vector<float> containing all float values from the branch.
 */
std::vector<float> readVectorFloatBranchFromRootFile(
    const std::string& filename, 
    const std::string& treename, 
    const std::string& branchname, 
    size_t maxBytes = 1'000'000'000
);


/**
 * @brief Reads all float values from a specified branch in multiple ROOT files.
 * @param filenames   Vector of ROOT file paths.
 * @param treename    Name of the tree in the files.
 * @param branchname  Name of the branch to read.
 * @param maxBytes    Maximum number of bytes to read (default: 1,000,000,000).
 * @return std::vector<float> containing all float values from the branch across files.
 */
std::vector<float> readVectorFloatBranchFromFiles(
    const std::vector<std::string>& filenames,
    const std::string& treename,
    const std::string& branchname,
    size_t maxBytes = 1'000'000'000
);


/**
 * @brief Writes decompressed data sets to a ROOT file as branches in a TTree.
 * @param filename Path to the ROOT file to write to.
 * @param treename Name of the tree to create or update.
 * @param data     Vector of DecompressedData sets to write as branches.
 */
void writeDecompressedDataToRootFile(
    const std::string& filename, 
    const std::string& treename,
    const std::vector<DecompressedData>& data
);  