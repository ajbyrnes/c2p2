#include "TApplication.h"
#include "compression_lib.hpp"

void truncateDistribution(std::vector<float>& basket, const int& min, const int& max, const std::string& distribution) {
    // Truncate data
    std::vector<std::vector<float>> truncatedBaskets;
    truncatedBaskets.push_back(basket);
    for (int i{1}; i <= 23; i++) {
        truncatedBaskets.push_back(truncateVectorData(basket, i));
    }

    // Create histograms
    std::vector<TH1F*> histograms;
    for (size_t i{0}; i < truncatedBaskets.size(); i++) {
        histograms.push_back(vectorToHistogram(truncatedBaskets[i], min, max, std::format("{}_{:02d}_bits", distribution, i)));
    }

    // Print histogram statistics
    for (size_t i{0}; i < histograms.size(); i++) {
        std::cout << std::format("{} with {:2} bits truncated: mean = {:.6f}, RMS = {:.6f}\n", distribution, i, histograms[i]->GetMean(), histograms[i]->GetRMS());
    }

    // Delete histograms
    for (TH1F* h : histograms) {
        delete h;
    }
}


void truncation(){
    // Generate data
    int basketSize{8000};
    int seed{12345};

    float min{0};
    float max{static_cast<float>(1 << 5)};

    // Uniform distribution
    std::uniform_real_distribution<float> uniformDistribution(min, max);
    std::vector<float> uniformBasket{generateRandomData(uniformDistribution, seed, basketSize)};

    // Truncate data and report results
    std::cout << "UNIFORM DISTRIBUTION" << std::endl;
    std::cout << "Min: " << min << std::endl;
    std::cout << "Max: " << max << std::endl;

    truncateDistribution(uniformBasket, min, max, "uniform");
    
    std::cout << "==========" << std::endl;

    // Normal distribution
    float mean{16.0};
    float sigma{4.0};
    std::normal_distribution<float> normalDistribution(mean, sigma);
    std::vector<float> normalBasket{generateRandomData(normalDistribution, seed, basketSize)};

    // Truncate data and report results
    std::cout << "NORMAL DISTRIBUTION" << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Sigma: " << sigma << std::endl;
    
    truncateDistribution(normalBasket, min, max, "normal");

    std::cout << "==========" << std::endl;
}

int main(int argc, char **argv) {
    TApplication app("app", 0, 0);
    truncation();
    app.Run();
    
    return 0;
}