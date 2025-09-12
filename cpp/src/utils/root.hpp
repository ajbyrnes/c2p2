
/**
 * @file root.hpp
 * @brief Declarations for reading and writing data to ROOT files.
 */
#pragma once

#include <vector>

/**
 * @brief Reads all values from a specified branch in a ROOT file.
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
); 