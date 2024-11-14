#include <cstdint>
#include <numeric>
#include <vector>

#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>

void bit_trunc_error_third() {
    // Set base value
    float val{1.0 / 3.0};
    uint32_t intVal{*reinterpret_cast<uint32_t*>(&val)};

    // Set original mask
    uint32_t mask{0xFFFFFFFF};

    // Shift mask, mask value
    std::vector<float> maskedVals{};
    for (int bitshift{0}; bitshift < 23; bitshift++) {
        mask = mask << 1;
        
        uint32_t maskedVal{intVal & mask};
        maskedVals.push_back(*reinterpret_cast<float*>(&maskedVal));
    }

    // Plot masked values
    std::vector<float> valVec(maskedVals.size(), val);  
    int numPoints{static_cast<int>(maskedVals.size())};

    std::vector<float> bits(23);
    std::iota(bits.begin(), bits.end(), 0);

    auto c = new TCanvas("c", "Masking Fraction Bits", 200, 10, 900, 700);

    auto mg = new TMultiGraph();
    auto g1 = new TGraph(numPoints, bits.data(), valVec.data());
    auto g2 = new TGraph(numPoints, bits.data(), maskedVals.data());

    g1->SetMarkerColor(kGreen);
    g1->SetLineColor(kGreen);
    g1->SetLineWidth(5);

    g2->SetMarkerColor(kRed);
    g2->SetLineColor(kRed);
    g2->SetLineStyle(2);
    g2->SetLineWidth(3);

    mg->Add(g1);
    mg->Add(g2);

    mg->SetTitle("Masking Fraction Bits;# Bits Masked;Value");
    mg->GetXaxis()->CenterTitle();
    mg->GetYaxis()->CenterTitle();

    mg->Draw("ALP");   
}