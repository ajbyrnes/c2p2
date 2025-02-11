#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

#include <iostream>

#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>

#include <bitset>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <limits>
#include <random>
#include <vector>

#include <iostream>
#include <iomanip>

#include <zlib.h>

#include <TAxis.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>

std::vector<unsigned char> compress(const std::vector<float>& input) {
    uLong inputBytes{input.size() * sizeof(float)};
    uLong outputSize{compressBound(inputBytes)};

    std::vector<unsigned char> output(outputSize);
    
    int result{compress2(output.data(), &outputSize, reinterpret_cast<const Bytef*>(input.data()), inputBytes, Z_BEST_COMPRESSION)};
    if (result != Z_OK) {
        throw std::runtime_error("Compression failed.");
    }

    output.resize(outputSize);
    return output;
}

std::vector<float> decompress(const std::vector<unsigned char>& input, size_t inputSize) {
    std::vector<float> output(inputSize / sizeof(float));
    uLong outputSize{inputSize};

    int result{uncompress(reinterpret_cast<Bytef*>(output.data()), &outputSize, input.data(), input.size())};
    if (result != Z_OK) {
        throw std::runtime_error("Decompression failed.");
    }

    return output;
}

std::vector<float> truncate(const std::vector<float>& input, uint32_t mask) {
    std::vector<float> output;
    output.reserve(input.size());
    for (float f : input) {
        uint32_t intVal{*reinterpret_cast<uint32_t*>(&f)};
        uint32_t maskedIntVal{intVal & mask};
        output.push_back(*reinterpret_cast<float*>(&maskedIntVal));
    }
    return output;
}

void printResults(std::string data, const std::vector<float>& uncompressed, const std::vector<unsigned char>& compressed) {
    std::cout << data << ": ";
    std::cout << "Uncompressed size: " << uncompressed.size() * sizeof(float) << " bytes; ";
    std::cout << "Compressed size: " << compressed.size() << " bytes; ";
    std::cout << "Uncompressed data and Compressed data " << (uncompressed == decompress(compressed, uncompressed.size() * sizeof(float)) ? "match." : "do not match.");
    std::cout << std::endl;

}

void bit_trunc_avgs() {
    // RNG setup
    // For uniform distribution over full range of floats
    std::random_device rd{};
    std::default_random_engine engine{rd()};
    std::uniform_real_distribution<float> dist{std::numeric_limits<float>::min(), std::numeric_limits<float>::max()};

    // Generate vectors of random floats
    const int numVecs{100};
    const int numRandFloats{1000};

    std::vector<std::vector<float>> vecs(numVecs, std::vector<float>(numRandFloats, 0.0));

    for (int v{0}; v < numVecs; v++) {
        for (int f{0}; f < numRandFloats; f++) {
            vecs[v][f] = dist(engine);
        }
    }

    // Set original mask
    uint32_t mask{0xFFFFFFFF};

    // Calculate truncated values, error, compression %
    const int uncompressedSize{numRandFloats * sizeof(float)};
    const int maxBitsMasked{23};

    std::vector<std::vector<std::vector<float>>> errorPct(maxBitsMasked + 1, std::vector<std::vector<float>>(numVecs, std::vector<float>(numRandFloats, 0.0)));
    std::vector<std::vector<float>> compressionPct(maxBitsMasked + 1, std::vector<float>(numVecs, 0.0));

    for (int b{0}; b <= 23; b++) {
        for (int v{0}; v < numVecs; v++) {
            // Truncate values
            std::vector<float> truncatedVec{truncate(vecs[v], mask)};
            
            // Calculate % error on each value
            for (int i{0}; i < numRandFloats; i++) {
                errorPct[b][v][i] = (std::abs((vecs[v][i] - truncatedVec[i]) / vecs[v][i]) * 100);
            }

            // Calculate compression % of vector
            std::vector<unsigned char> compressedVec{compress(truncatedVec)};
            compressionPct[b][v] = (static_cast<float>(compressedVec.size()) / uncompressedSize) * 100;
        }

        // Shift mask to truncate more bits
        mask = mask << 1;
    }

    // Calculate average % error, compression %
    std::vector<float> avgErrorPct(maxBitsMasked + 1, 0.0);
    std::vector<float> avgCompressionPct(maxBitsMasked + 1, 0.0);

    for (int b{0}; b <= 23; b++) {
        // Average % error is taken over all values in all vectors
        float avgErrorPctB{0.0};
        for (int v{0}; v < numVecs; v++) {
            avgErrorPctB += std::accumulate(errorPct[b][v].begin(), errorPct[b][v].end(), 0.0);
        }
        avgErrorPctB /= numRandFloats;
        avgErrorPct[b] = avgErrorPctB / numVecs;

        // Average compression % is taken over all vectors
        avgCompressionPct[b] = std::accumulate(compressionPct[b].begin(), compressionPct[b].end(), 0.0) / numVecs;
    }

    // Print average % error, compression %
    std::cout << "Average % Error and Compression % for Truncation of Fraction Bits" << std::endl;
    std::cout << std::setw(5) << "Bits Masked" << std::setw(15) << "Avg % Error" << std::setw(20) << "Avg Compression %" << std::endl;
    for (int b{0}; b <= 23; b++) {
        std::cout << std::fixed;
        std::cout << std::setw(5) << b << std::setw(15) << avgErrorPct[b] << std::setw(20) << avgCompressionPct[b] << std::endl;
    }

    // Plot average % error, compression %
    std::vector<float> bitsMasked(maxBitsMasked + 1);
    std::iota(bitsMasked.begin(), bitsMasked.end(), 0);

    auto c = new TCanvas("c", "%", 200, 10, 900, 700);
    auto mg = new TMultiGraph();

    auto eg = new TGraph(maxBitsMasked, bitsMasked.data(), avgErrorPct.data());
    auto cg = new TGraph(maxBitsMasked, bitsMasked.data(), avgCompressionPct.data());

    eg->SetMarkerColor(kRed);
    eg->SetLineColor(kRed);
    eg->SetLineWidth(5);

    cg->SetMarkerColor(kBlue);
    cg->SetLineColor(kBlue);
    cg->SetLineWidth(5);

    eg->SetName("eg");
    cg->SetName("cg");

    auto leg = new TLegend();
    leg->AddEntry(eg, "Avg % Error", "l");
    leg->AddEntry(cg, "Avg Compression %", "l");

    mg->Add(eg);
    mg->Add(cg);

    mg->SetTitle("Average % Error and Compression %;# Bits Masked;Avg % Error and Compression %");
    mg->GetXaxis()->CenterTitle();
    mg->GetYaxis()->CenterTitle();

    mg->Draw("ALP");
    leg->Draw();
}
