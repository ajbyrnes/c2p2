#include <format>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include <unistd.h>

#include <TFile.h>
#include <TError.h>
#include <TTree.h>

#include "utils.hpp"

const Args parseArgs(int argc, char* argv[]) {
    Args args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--input-file") {
            if (i + 1 < argc) {
                args.inputFile = argv[++i];
            } else {
                throw std::runtime_error("Error: --input-file requires a value");
            }
        }
        else if (arg == "--tree") {
            if (i + 1 < argc) {
                args.treename = argv[++i];
            } else {
                throw std::runtime_error("Error: --tree requires a value");
            }
        }
        else if (arg == "--branch") {
            if (i + 1 < argc) {
                args.branchname = argv[++i];
            } else {
                throw std::runtime_error("Error: --branch requires a value");
            }
        }
        else if (arg == "--max-bytes") {
            if (i + 1 < argc) {
                args.maxBytes = std::stoul(argv[++i]);
            } else {
                throw std::runtime_error("Error: --max-bytes requires a value");
            }
        }
        else if (arg == "--compressor") {
            if (i + 1 < argc) {
                args.compressor = argv[++i];
            } else {
                throw std::runtime_error("Error: --compressor requires a value");
            }
        }
        else if (arg == "--iterations") {
            if (i + 1 < argc) {
                args.iterations = std::stoi(argv[++i]);
            } else {
                throw std::runtime_error("Error: --iterations requires a value");
            }
        }
        else if (arg == "--mantissa-bits") {
            if (i + 1 < argc) {
                args.mantissaBits = std::stoi(argv[++i]);
            } else {
                throw std::runtime_error("Error: --mantissa-bits requires a value");
            }
        } else if (arg == "--compression-level") {
            if (i + 1 < argc) {
                args.compressionLevel = std::stoi(argv[++i]);
            } else {
                throw std::runtime_error("Error: --compression-level requires a value");
            }
        } else {
            throw std::runtime_error("Unknown argument: " + arg);
        }
    }

    return args;
}

std::vector<float> readVectorFloatBranchFromFile(
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

    std::cout << std::format("[{}] Reading {} entries from branch '{}' in file '{}'...'", getTimestamp(), nEntries, branchname, filename);
    std::cout << std::endl;

    for (Long64_t i = 0; i < nEntries; ++i) {
        tree->GetEntry(i);

        // Push entries into flatData
        for (const auto& value : *entryData) {
            flatData.push_back(value);
        }

        // Check if we exceed the maximum bytes limit
        if (flatData.size() * sizeof(float) >= maxBytes) {
            std::cout << std::format("[{}] Reached maximum size of {} bytes, stopping read at entry {}", getTimestamp(), maxBytes, i);
            std::cout << std::endl;
            break;
        }
    }


    file->Close();
    delete file;

    // Size can be reported in GB, MB, KB, or bytes
    int numBytes = flatData.size() * sizeof(float);
    std::cout << std::format("[{}] Read {} values ({}) from {} entries from branch '{}'", 
                            getTimestamp(), flatData.size(), getSizeString(numBytes), nEntries, branchname);
    std::cout << std::endl;

    return flatData;
}

std::vector<float> readVectorFloatBranchFromFiles(
    const std::vector<std::string>& filenames,
    const std::string& treename,
    const std::string& branchname,
    size_t maxBytes)
{
    std::vector<float> flatData;
    flatData.reserve(maxBytes / sizeof(float)); // Preallocate

    for (const auto& filename : filenames) {
        size_t bytesLeft = maxBytes - flatData.size() * sizeof(float);
        if (bytesLeft == 0) break;

        std::vector<float> data = readVectorFloatBranchFromFile(filename, treename, branchname, bytesLeft);

        if (flatData.capacity() < flatData.size() + data.size()) {
            flatData.reserve(flatData.size() + data.size());
        }

        size_t floatsLeft = bytesLeft / sizeof(float);
        if (data.size() > floatsLeft) {
            data.resize(floatsLeft);

        }
        flatData.insert(flatData.end(), data.begin(), data.end());

        if (flatData.size() * sizeof(float) >= maxBytes) break;
    }

    std::cout << std::format("[{}] Read {} values ({}) from branch '{}' across {} files", 
                            getTimestamp(), flatData.size(), getSizeString(flatData.size() * sizeof(float)), branchname, filenames.size());
    std::cout << std::endl;

    return flatData;
}

void writeFloatVectorToFile(
    const std::string& filename, 
    const std::vector<float>& data, 
    const std::string& treename,
    const std::string& branchname,
    bool recreate)
{
    // Open the ROOT file
    TFile file(filename.c_str(), recreate ? "RECREATE" : "UPDATE");
    if (!file.IsOpen()) {
        throw std::runtime_error("Failed to open file for writing");
    }

    // Write to existing tree or create a new one if it doesn't exist
    TTree* tree = dynamic_cast<TTree*>(file.Get(treename.c_str()));
    if (!tree) {
        tree = new TTree(treename.c_str(), "Tree containing float vector data");
        std::cout << timeMessage(std::format("Created new tree '{}' in file '{}'", treename, filename)) << std::endl;
    }

    // Create a local vector and set branch address for writing
    std::vector<float> branchData = data;
    tree->Branch(branchname.c_str(), &branchData);

    // Only fill the new branch (other branches' addresses are not set, so they are not filled)
    tree->Fill();

    // Write the tree to the file
    tree->Write();

    // Close the file
    file.Close();

    std::cout << std::format("[{}] Wrote {} values ({}) to branch '{}' in tree '{}' in file '{}'", 
                            getTimestamp(), data.size(), getSizeString(data.size() * sizeof(float)), branchname, treename, filename);
    std::cout << std::endl;
}

std::string getSizeString(size_t numBytes) {
    if (numBytes >= 1024 * 1024 * 1024) {
        return std::format("{:.2f} GB", numBytes / (1024.0 * 1024.0 * 1024.0));
    } else if (numBytes >= 1024 * 1024) {
        return std::format("{:.2f} MB", numBytes / (1024.0 * 1024.0));
    } else if (numBytes >= 1024) {
        return std::format("{:.2f} KB", numBytes / 1024.0);
    } else {
        return std::to_string(numBytes) + " bytes";
    }
}

std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm = *std::localtime(&in_time_t);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    
    return std::string(buffer);
}

std::string getHost() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        return std::string(hostname);
    } else {
        return "unknown_host";
    }
}

std::string timeMessage(const std::string& message) {
    return std::format("[{}] {}", getTimestamp(), message);
}