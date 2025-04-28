#ifndef LIB_UTILS_HPP
#define LIB_UTILS_HPP

#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <random>
#include <vector>
#include <unistd.h>

#include <TFile.h>
#include <TTree.h>

// Constants -----------------------------------------------------------------------------------------------------

constexpr size_t KB{1'000};
constexpr size_t MB{1'000'000};

// Read ROOT file ----------------------------------------------------------------------------------
std::vector<float> readRootFile(const size_t size, const std::string& filename, const std::string& treeName, const std::string& branchName) {
    std::cerr << std::format("[DEBUG benchmark] Reading ROOT file: \"{}\"", filename) << std::endl;

    // Open ROOT file
    TFile* file = TFile::Open(filename.c_str());
    if (!file || file->IsZombie()) {
        throw std::runtime_error("Failed to open ROOT file: " + filename);
        return {};
    }

    // Load tree from file
    TTree* tree = static_cast<TTree*>(file->Get(treeName.c_str()));

    // Read data from tree into vector
    if (size) {
        throw std::runtime_error("Size limit not implemented for reading ROOT file");
        return {};
    }

    std::vector<float> data{};
    std::vector<float>* entry = nullptr;
    tree->SetBranchAddress(branchName.c_str(), &entry);

    size_t numEntries = tree->GetEntries();

    std::cerr << std::format("[DEBUG benchmark] Loading data from tree {} branch {}", treeName, branchName) << std::endl;
    for (size_t n = 0; n < numEntries; ++n) {
        // Load next entry
        tree->GetEntry(n);

        // Print entry
        for (size_t j = 0; j < entry->size(); ++j) {
            data.push_back((*entry)[j]);
        }
    }

    // Return vector
    return data;
}


// Data generation ----------------------------------------------------------------------------------
std::vector<float> generateUniformRandomData(size_t size, float min, float max) {
    std::vector<float> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);

    for (size_t i = 0; i < size; ++i) {
        data[i] = dis(gen);
    }

    return data;
}

std::vector<float> generateGaussianRandomData(size_t size, float mean, float stddev, int seed) {
    std::vector<float> data(size);
    std::random_device rd{};
    std::mt19937 gen(seed);
    std::normal_distribution<float> dis(mean, stddev);

    for (size_t i = 0; i < size; ++i) {
        data[i] = dis(gen);
    }

    return data;
}

// Other utilities ---------------------------------------------------------------------------------------------------
std::string getHost() {
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    return std::string(hostname);
}

std::string timestamp() {
    return std::format("{}", std::chrono::system_clock::now().time_since_epoch().count());
}

// Benchmarking ------------------------------------------------------------------------------------------------------



#endif