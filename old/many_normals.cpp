
#include <iostream>
#include <format>
#include <numeric>
#include <vector>

#include <TApplication.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TLegend.h>
#include <TRandom2.h>

#include <zlib.h>
#include "utils.hpp"

constexpr int MANTISSA{23};

struct Normal{
    std::vector<float> data{};
    std::vector<TH1F*> histograms{};
    float mean{};
    float stddev{};
};

TH1F* makeHisto(std::vector<float>& data, int nBitsMasked, int iteration, float mean, float sigma) {
    // Fill histogram with data from vector
    TH1F* h{new TH1F(std::format("hist_{}_{}", iteration, nBitsMasked).c_str(), "Histogram", 
                                100, 
                                sigma - 3 * sigma, 
                                sigma + 3 * sigma)};
    
    for (const float& f : data) {
        h->Fill(f);
    }

    return h;
}

Normal getTruncatedNormal(int iteration) {
    Normal normal{};

    // Generate random mean and sigma
    TRandom2* rand{new TRandom2()};
    normal.mean = rand->Rndm();
    normal.stddev = rand->Rndm();

    // Fill vector of floats with random data from Gaussian distribution
    for (int i{0}; i < 10'000; i++) {
        normal.data.push_back(rand->Gaus(normal.mean, normal.stddev));
    }

    // Make histogram from original data
    TH1F* h{makeHisto(normal.data, iteration, 0, normal.mean, normal.stddev)};
    normal.histograms.push_back(h);

    // Truncate progressively more of the mantissa
    // Create histogram for each set of truncated data
    for (int i{1}; i <= MANTISSA; i++) {
        // Truncate
        std::vector<float> truncatedData{truncate(normal.data, i)};

        // Create histogram
        TH1F* h{makeHisto(truncatedData, iteration, i, normal.mean, normal.stddev)};
        normal.histograms.push_back(h);
    }

    return normal;
}

int main() {
    // Application setup
    TApplication app("app", 0, 0);

    // Generate random normal distributions
    // Truncate data and create histograms
    std::vector<Normal> normals{};
    for (int i{0}; i < 1000; i++) {
        normals.push_back(getTruncatedNormal(i));
    }

    // For each level of truncation and normal distribution,
    // calculate difference between actual mean and truncated mean
    // and actual stddev and truncated stddev
    std::vector<std::vector<float>> distanceFromMean(MANTISSA + 1, std::vector<float>(normals.size()));
    std::vector<std::vector<float>> distanceFromStdDev(MANTISSA + 1, std::vector<float>(normals.size()));

    for (size_t b{0}; b < MANTISSA + 1; b++) {      // Truncation index
        for (size_t n{0}; n < normals.size(); n++) {        // Normal distribution index
            distanceFromMean[b][n] = (normals[n].mean - normals[n].histograms[b]->GetMean());
            distanceFromStdDev[b][n] = (normals[n].stddev - normals[n].histograms[b]->GetStdDev());
        }
    }

    // For each level of truncation, calculate average difference
    // between actual mean and truncated mean and actual stddev and truncated stddev
    std::vector<float> avgDistanceFromMean(MANTISSA + 1, 0.0);
    std::vector<float> avgDistanceFromStdDev(MANTISSA + 1, 0.0);
    
    for (size_t b{0}; b < MANTISSA + 1; b++) {
        avgDistanceFromMean[b] = std::accumulate(distanceFromMean[b].begin(), distanceFromMean[b].end(), 0.0) / normals.size();
        avgDistanceFromStdDev[b] = std::accumulate(distanceFromStdDev[b].begin(), distanceFromStdDev[b].end(), 0.0) / normals.size();
    }

    // Get x values for means and stddev plots
    std::vector<float> bitsMasked(MANTISSA + 1);
    std::iota(bitsMasked.begin(), bitsMasked.end(), 0);

    // Get y values for means plot
    TGraph* grMeans{new TGraph(MANTISSA + 1, bitsMasked.data(), avgDistanceFromMean.data())};

    // Get y values for stddev plot
    TGraph* grStdDev{new TGraph(MANTISSA + 1, bitsMasked.data(), avgDistanceFromStdDev.data())};

    // Draw means ---------------------------------------------------------------------------
    TCanvas* cMeans{new TCanvas()};
    grMeans->SetMarkerStyle(20);
    grMeans->SetMarkerSize(1.5);
    grMeans->SetMarkerColor(kRed);
    grMeans->SetLineColor(kRed);
    grMeans->SetLineWidth(2);
    grMeans->SetTitle("Distance from Mean;Bits Masked;Mean");
    grMeans->GetXaxis()->CenterTitle();
    grMeans->GetYaxis()->CenterTitle();
    grMeans->Draw("APL");

    // Draw stddev ---------------------------------------------------------------------------
    TCanvas* cStdDev{new TCanvas()};
    grStdDev->SetMarkerStyle(20);
    grStdDev->SetMarkerSize(1.5);
    grStdDev->SetMarkerColor(kRed);
    grStdDev->SetLineColor(kRed);
    grStdDev->SetLineWidth(2);
    grStdDev->SetTitle("Distance from StdDev;Bits Masked;StdDev");
    grStdDev->GetXaxis()->CenterTitle();
    grStdDev->GetYaxis()->CenterTitle();
    grStdDev->Draw("APL");

    app.Run();
    return 0;
}

