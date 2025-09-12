#include <vector>
#include <iostream>
#include <string>
#include <random>

#include <TFile.h>
#include <TTree.h>

int main() {
    // Create dummy data
    const int nEntries = 100;
    std::vector<std::vector<float>> data;
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_real_distribution<float> dist(0.0, 100.0);
    
    for (int i = 0; i < nEntries; ++i) {
        std::vector<float> entry{};
        int nJets = rng() % 10; // Random number of jets per entry
        for (int j = 0; j < nJets; ++j) {
            entry.push_back(dist(rng));
        }
        data.push_back(std::move(entry));
    }

    // Step 1: Write a TTree with one branch
    TFile* file = TFile::Open("test.root", "RECREATE");
    TTree* tree = new TTree("CollectionTree", "A tree with jet pT");
    std::vector<float>* branchData = nullptr;
    tree->Branch("AnalysisJetsAuxDyn_pt", &branchData);
    for (int i = 0; i < nEntries; ++i) {
        branchData = &data[i];
        tree->Fill();
    }
    tree->Write();
    file->Close();
    delete file;

    // Step 2: Reopen file, add a new branch, and fill it so both branches share the same index
    file = TFile::Open("test.root", "UPDATE");
    tree = dynamic_cast<TTree*>(file->Get("CollectionTree"));
    if (!tree) {
        std::cerr << "Failed to get tree" << std::endl;
        return 1;
    }

    // Create new branch to copy vector<float> data
    std::vector<float> newBranchValue;
    TBranch* newBranch = tree->Branch("NewBranch", &newBranchValue);

    // Loop over all entries, fill only the new branch
    for (Long64_t i = 0; i < tree->GetEntries(); ++i) {
        tree->GetEntry(i); // loads the entry for all existing branches
        newBranchValue = data[i]; // or any logic you want
        std::cout << "Filling entry " << i << " with NewBranch value: " << newBranchValue.size() << std::endl;
        newBranch->Fill(); // fills only the new branch for this entry
    }

    tree->Write("", TObject::kOverwrite);
    file->Close();
    delete file;
}