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

const std::string filepath = "/home/abelo/data/atlas/mc_361106.Zee.1largeRjet1lep.root";

const std::vector<std::string> floatBranches = {
    "mcWeight",
    "met_et_syst",
    "met_et",
    "met_phi",
    "scaleFactor_BTAG",
    "scaleFactor_ELE",
    "scaleFactor_LepTRIGGER",
    "scaleFactor_MUON",
    "scaleFactor_PHOTON",
    "scaleFactor_PhotonTRIGGER",
    "scaleFactor_PILEUP",
    "scaleFactor_TAU",
};

const std::vector<std::string> vectorFloatBranches = {
    "jet_E",
    "jet_eta",
    "jet_jvt",
    "jet_MV2c10",
    "jet_phi",
    "jet_pt_syst",
    "jet_pt",
    "largeRjet_D2",
    "largeRjet_E",
    "largeRjet_eta",
    "largeRjet_m",
    "largeRjet_phi",
    "largeRjet_pt_syst"
    "largeRjet_pt",
    "largeRjet_tau32",
    "largeRjet_truthMatched",
    "lep_E",
    "lep_eta",
    "lep_etcone20",
    "lep_phi",
    "lep_pt_syst",
    "lep_pt",
    "lep_ptcone30",
    "lep_trackd0pvunbiased",
    "lep_tracksigd0pvunbiased",
    "lep_z0"
};

// Read ROOT file ----------------------------------------------------------------------------------
std::vector<float> readRootFile(const size_t size, const std::string& filename, const std::string& treeName, const std::string& branchName, bool debug=false) {
    // Open ROOT file
    if (debug) std::cerr << std::format("[DEBUG benchmark] Reading ROOT file: \"{}\"", filename) << std::endl;
    TFile* file = TFile::Open(filename.c_str());
    if (!file || file->IsZombie()) {
        throw std::runtime_error("Failed to open ROOT file: " + filename);
        return {};
    }

    // Load tree from file
    if (debug) std::cerr << std::format("[DEBUG benchmark] Loading tree: \"{}\"", treeName) << std::endl;
    TTree* tree = static_cast<TTree*>(file->Get(treeName.c_str()));

    // Read data from tree into vector
    if (size) {
        throw std::runtime_error("Size limit not implemented for reading ROOT file");
        return {};
    }

    // Create vector to hold flattened data
    std::vector<float> data{};
    
    // Get number of entries in the tree
    const size_t numEntries = tree->GetEntries();

    // Each entry is either a float or a vector of floats
    if (std::find(floatBranches.begin(), floatBranches.end(), branchName) != floatBranches.end()) {
        // If the branch is a float, set the branch address to a float pointer
        float* entry = nullptr;
        tree->SetBranchAddress(branchName.c_str(), &entry);

        // Loop over entries and read data into vector
        if (debug) std::cerr << std::format("[DEBUG benchmark] Loading float data from tree {} branch {}", treeName, branchName) << std::endl;
        for (size_t n = 0; n < numEntries; ++n) {
            if (debug) std::cerr << std::format("[DEBUG benchmark] Loading entry {}", n) << std::endl;
            // Load next entry
            tree->GetEntry(n);

            if (debug) std::cerr << std::format("[DEBUG benchmark] Entry value: {}", *entry) << std::endl;
            // Print entry
            data.push_back(*entry);
        }
    } else if (std::find(vectorFloatBranches.begin(), vectorFloatBranches.end(), branchName) != vectorFloatBranches.end()) {
        // If the branch is a vector of floats, set the branch address to a vector pointer
        std::vector<float>* entry = nullptr;
        tree->SetBranchAddress(branchName.c_str(), &entry);

        // Loop over entries and read data into vector
        if (debug) std::cerr << std::format("[DEBUG benchmark] Loading float-vector data from tree {} branch {}", treeName, branchName) << std::endl;
        for (size_t n = 0; n < numEntries; ++n) {
            // Load next entry
            tree->GetEntry(n);

            // Print entry
            for (size_t j = 0; j < entry->size(); ++j) {
                data.push_back((*entry)[j]);
            }
        }
    } else {
        throw std::runtime_error("Invalid branch name: " + branchName);
        return {};
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

#endif