
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

constexpr int MEAN{0};
constexpr int SIGMA{1};

TH1F* makeHisto(std::vector<float>& data, int n) {
    // Fill histogram with data from vector
    TH1F* h{new TH1F(std::format("hist_{}", n).c_str(), "Histogram", 
                                100, 
                                MEAN - 3 * SIGMA, 
                                MEAN + 3 * SIGMA)};
    
    for (const float& f : data) {
        h->Fill(f);
    }

    return h;
}

int main() {
    // Application setup
    TApplication app("app", 0, 0);

    // Fill vector of floats with random data from Gaussian distribution
    TRandom2* rand{new TRandom2()};
    std::vector<float> data{};

    for (int i{0}; i < 10'000; i++) {
        data.push_back(rand->Gaus(MEAN, SIGMA));
    }

    // Create vector to store histograms
    std::vector<TH1F*> hists{};

    // Draw histogram of original data
    TH1F* h{makeHisto(data, 0)};
    hists.push_back(h);

    // Truncate progressively more of the mantissa
    // Create histogram for each set of truncated data
    constexpr int mantissa{23};
    for (int i{1}; i <= mantissa; i++) {
        // Truncate
        std::vector<float> truncatedData{truncate(data, i)};

        // Print first element of truncated data
        std::cout << std::format("{:2} {:.20f}", i, truncatedData[0]) << std::endl;

        // Create histogram
        TH1F* h{makeHisto(truncatedData, i)};
        hists.push_back(h);
    }


    // const std::vector<int> colors{
    //     kBlue, kBlue + 4,
    //     kAzure, kAzure - 7,
    //     kCyan, kCyan + 4,
    //     kTeal, kTeal - 7,
    //     kGreen, kGreen + 4,
    //     kSpring, kSpring - 7,
    //     kYellow, kYellow + 4,
    //     kOrange, kOrange - 7,
    //     kRed, kRed + 4,
    //     kPink, kPink - 7,
    //     kMagenta, kMagenta + 4,
    //     kViolet, kViolet - 7
    // };
    
    // Draw histograms ---------------------------------------------------------------------------
    TCanvas* cHist{new TCanvas()};

    // std::vector<int> selection{0, 2, 4, 8, 16, 18, 22};
    // std::vector<int> selection{22};
    std::vector<int> selection{0, 16, 19, 20, 21, 22};
    std::vector<int> colors{
        kBlack, kCyan, kGreen, kViolet, kMagenta, kRed
    };

    float max{0};
    for (size_t i{}; i < selection.size(); i++) {
        if (hists[selection[i]]->GetMaximum() > max) {
            max = hists[selection[i]]->GetMaximum();
        }

        hists[selection[i]]->SetLineColor(colors[i]);
        hists[selection[i]]->SetStats(false);
        hists[selection[i]]->Draw("SAME");
    }

    hists[0]->SetFillColor(kBlack);

    // Adjust range
    hists[0]->GetYaxis()->SetRangeUser(0, max * 1.05);

    // Create legend
    TLegend* legend{new TLegend(0.7, 0.7, 0.9, 0.9)};
    for (const int& i : selection) {
        legend->AddEntry(hists[i], hists[i]->GetName(), "l");
    }
    legend->Draw();

    // Save image
    // c->Print("normal.png");

    // Draw means ---------------------------------------------------------------------------
    TCanvas* cMeans{new TCanvas()};

    // Get x values for means plot
    std::vector<float> bitsMasked(mantissa + 1);
    std::iota(bitsMasked.begin(), bitsMasked.end(), 0);

    // Print bitsmasked
    for (const float& f : bitsMasked) {
        std::cout << f << std::endl;
    }

    // Get y values for means plot
    std::vector<float> means{};
    for (const TH1F* hist : hists) {
        means.push_back(hist->GetMean());
    }

    TGraph* grMeans{new TGraph(mantissa + 1, bitsMasked.data(), means.data())};
    grMeans->SetMarkerStyle(20);
    grMeans->SetMarkerSize(1.5);
    grMeans->SetMarkerColor(kRed);
    grMeans->SetLineColor(kRed);
    grMeans->SetLineWidth(2);
    grMeans->SetTitle("Mean of Truncated Data;Bits Masked;Mean");
    grMeans->GetXaxis()->CenterTitle();
    grMeans->GetYaxis()->CenterTitle();
    grMeans->Draw("APL");

    // Draw stddev ---------------------------------------------------------------------------
    TCanvas* cStdDev{new TCanvas()};

    // Get y values for means plot
    std::vector<float> stds{};
    for (const TH1F* hist : hists) {
        stds.push_back(hist->GetStdDev());
    }

    TGraph* grStdDev{new TGraph(bitsMasked.size(), bitsMasked.data(), stds.data())};
    grStdDev->SetMarkerStyle(20);
    grStdDev->SetMarkerSize(1.5);
    grStdDev->SetMarkerColor(kRed);
    grStdDev->SetLineColor(kRed);
    grStdDev->SetLineWidth(2);
    grStdDev->SetTitle("StdDev of Truncated Data;Bits Masked;StdDev");
    grStdDev->GetXaxis()->CenterTitle();
    grStdDev->GetYaxis()->CenterTitle();
    grStdDev->Draw("APL");

    app.Run();
    return 0;
}

