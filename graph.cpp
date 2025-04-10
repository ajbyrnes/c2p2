#include <cmath>

#include <TAxis.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TPad.h>

#include "lib/lib_utils.hpp"
#include "lib/lib_zlib.hpp"
#include "lib/lib_sz.hpp"

void graph() {
    // Set data parameters
    int NUMFLOATS{1 * MB / sizeof(float)};
    double MIN{-50.0}, MAX{50.0};
    double NOISE{0.75};
    int TEST{4}; // 1 for uniform random data

    std::vector<float> xData{generateXData(NUMFLOATS, MIN, MAX)};
    CompressorIn testData{generateTestData(TEST, NUMFLOATS, MIN, MAX, NOISE)};

    // Set compression parameters
    int PRECISION{3};
    int BITS_TRUNCATED{calculateBitsToTruncate(PRECISION)};
    double RELATIVE_ERROR{calculateRelativeError(PRECISION)};

    int COMPRESSION_LEVEL{1};
    
    std::vector<SZ3::ALGO> algorithms = {
        SZ3::ALGO_LORENZO_REG,
        SZ3::ALGO_INTERP_LORENZO,
        SZ3::ALGO_INTERP,
        SZ3::ALGO_NOPRED,
        SZ3::ALGO_LOSSLESS
    };

    std::vector<SZ3::INTERP_ALGO> interpAlgorithms = {
        SZ3::INTERP_ALGO_LINEAR,
        SZ3::INTERP_ALGO_CUBIC
    };

    CompressorParams zlibParams{
        {"compressionLevel", COMPRESSION_LEVEL},
        {"bitsTruncated", BITS_TRUNCATED}
    };

    CompressorParams params{
        {"relativeError", RELATIVE_ERROR},
        {"errorBoundMode", SZ3::EB_REL},
        {"algo", algorithms[0]},
        {"interpAlgo", interpAlgorithms[0]}
    };

    // Compress data
    CompressorOut compZlibData{zlibTruncateCompress(testData, zlibParams)};
    DecompressorOut zlibData{zlibDecompress(compZlibData, NUMFLOATS)};

    CompressorOut compSzData{szCompress(testData, params)};
    DecompressorOut szData{szDecompress(compSzData, NUMFLOATS)};

    // Calculate difference between original and decompressed data
    std::vector<float> zlibDiffData(NUMFLOATS);
    std::vector<float> szDiffData(NUMFLOATS);

    for (size_t i = 0; i < NUMFLOATS; ++i) {
        zlibDiffData[i] = std::abs(testData[i] - zlibData[i]);
        szDiffData[i] = std::abs(testData[i] - szData[i]);
    }

    // Choose random sample of data to graph
    std::mt19937 gen(1234);
    std::uniform_int_distribution<size_t> dist(0, NUMFLOATS - 1);

    int SAMPLE_SIZE{100};
    std::vector<size_t> randomIndices(SAMPLE_SIZE);

    for (size_t i = 0; i < randomIndices.size(); ++i) {
        randomIndices[i] = dist(gen);
    }
    std::sort(randomIndices.begin(), randomIndices.end());

    std::vector<float> sampleXData(SAMPLE_SIZE);
    CompressorIn sampleTestData(SAMPLE_SIZE);
    DecompressorOut sampleZlibData(SAMPLE_SIZE);
    DecompressorOut sampleSzData(SAMPLE_SIZE);

    for (size_t i = 0; i < randomIndices.size(); ++i) {
        sampleXData[i] = xData[randomIndices[i]];
        sampleTestData[i] = testData[randomIndices[i]];
        sampleZlibData[i] = zlibData[randomIndices[i]];
        sampleSzData[i] = szData[randomIndices[i]];
    }

    // Create canvases
    TCanvas* c1{new TCanvas("c1", "all data", 900, 700)};
    TCanvas* c2{new TCanvas("c2", "sampled Data", 900, 700)};
    // TCanvas* c3{new TCanvas("c3", "Difference", 900, 700)};

    // Make graphs of test data and decompressed data
    TGraph* gTestData{new TGraph(NUMFLOATS, xData.data(), testData.data())};
    TGraph* gSampleTestData{new TGraph(SAMPLE_SIZE, sampleXData.data(), sampleTestData.data())};

    TGraph* gZlibData{new TGraph(NUMFLOATS, xData.data(), zlibData.data())};
    TGraph* gSampleZlibData{new TGraph(SAMPLE_SIZE, sampleXData.data(), sampleZlibData.data())};
    TGraph* gSzData{new TGraph(NUMFLOATS, xData.data(), szData.data())};
    TGraph* gSampleSzData{new TGraph(SAMPLE_SIZE, sampleXData.data(), sampleSzData.data())};

    TGraph* gZlibDiffData{new TGraph(NUMFLOATS, xData.data(), zlibDiffData.data())};
    TGraph* gSzDiffData{new TGraph(NUMFLOATS, xData.data(), szDiffData.data())};

    // Style and draw the graphs
    gTestData->SetTitle("Test Data");
    gSampleTestData->SetTitle("Sampled Test Data");

    gZlibData->SetTitle("Decompressed Data: Bit Truncation + Zlib");
    gZlibData->SetLineColor(kRed);

    gSampleZlibData->SetTitle("Sampled Decompressed Data: Bit Truncation + Zlib");
    gSampleZlibData->SetLineColor(kRed);

    gSzData->SetTitle("Decompressed Data: SZ3");
    gSzData->SetLineColor(kGreen);

    gSampleSzData->SetTitle("Sampled Decompressed Data: SZ3");
    gSampleSzData->SetLineColor(kGreen);

    // Draw the graphs
    c1->Divide(1, 3);
    c1->cd(1);
    gTestData->Draw("AL");
    c1->cd(2);
    gZlibData->Draw("AL");
    c1->cd(3);
    gSzData->Draw("AL");

    c2->Divide(1, 3);
    c2->cd(1);
    gSampleTestData->Draw();
    c2->cd(2);
    gSampleZlibData->Draw();
    c2->cd(3);
    gSampleSzData->Draw();

    // Write the canvases to a file
    c1->SaveAs("graph_allData.png");
    c2->SaveAs("graph_sampleData.png");
}

int main() {
    graph();
    return 0;   
}