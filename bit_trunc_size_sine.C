#include <bitset>
#include <cmath>
#include <cstdint>
#include <numeric>
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

void bit_trunc_sine() {
    // Create float data
    std::vector<float> x{};
    std::vector<float> y{};

    std::vector<float> xCompressedSizes{};
    std::vector<float> yCompressedSizes{};

    int numPoints{100};
    float stepSize{static_cast<float>((2 * M_PI) / numPoints)};

    for (float f{0}; f <= (2 * M_PI); f += stepSize) {
        x.push_back(f);
        y.push_back(std::sin(f));
    }

    // Compress untruncated vectors
    std::vector<unsigned char> compressedX{compress(x)};
    std::vector<unsigned char> compressedY{compress(y)};

    // Record sizes
    xCompressedSizes.push_back(compressedX.size());
    yCompressedSizes.push_back(compressedY.size());

    // Test effect of truncating more bits
    uint32_t mask{0xFFFFFFFF};
    for (int bitshift{0}; bitshift < 23; bitshift++) {
        // Truncate values in vectors
        mask = mask << 1;
        std::vector<float> truncatedX{truncate(x, mask)};
        std::vector<float> truncatedY{truncate(y, mask)};

        // Compress truncated vectors
        std::vector<unsigned char> compressedTruncatedX{compress(truncatedX)};
        std::vector<unsigned char> compressedTruncatedY{compress(truncatedY)};

        printResults("truncatedX (" + std::to_string(bitshift) + " bits)", truncatedX, compressedTruncatedX);
        printResults("truncatedY (" + std::to_string(bitshift) + " bits)", truncatedY, compressedTruncatedY);
        std::cout << std::endl;

        // Record sizes
        xCompressedSizes.push_back(compressedTruncatedX.size());
        yCompressedSizes.push_back(compressedTruncatedY.size());
    }

    // Plot 
    const int numGraphPoints{static_cast<int>(xCompressedSizes.size())};

    std::vector<float> bits(numGraphPoints);
    std::iota(bits.begin(), bits.end(), 0);

    std::vector<float> originalSize(numGraphPoints, x.size() * sizeof(float));

    auto c = new TCanvas("c", "Masking Fraction Bits", 200, 10, 900, 700);
    auto mg = new TMultiGraph();   
    auto og = new TGraph(numGraphPoints, bits.data(), originalSize.data());
    auto xg = new TGraph(numGraphPoints, bits.data(), xCompressedSizes.data());
    auto yg = new TGraph(numGraphPoints, bits.data(), yCompressedSizes.data());

    og->SetLineWidth(5);

    xg->SetLineColor(kBlue);
    xg->SetLineStyle(2);
    xg->SetLineWidth(3);

    yg->SetLineColor(kRed);
    yg->SetLineStyle(2);
    yg->SetLineWidth(3);

    mg->Add(og);
    mg->Add(xg);
    mg->Add(yg);

    mg->Draw("ALP");
}