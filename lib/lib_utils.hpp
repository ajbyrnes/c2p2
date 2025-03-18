#include <chrono>
#include <fstream>
#include <random>
#include <vector>

#include <unistd.h>

// #include <TH1F.h>

#ifndef LIB_UTILS_HPP
#define LIB_UTILS_HPP

constexpr int MB{1'000'000};
constexpr int KB{1'000};

struct CompressionResults {
    std::vector<uint8_t> compressedData{};
    std::chrono::duration<long, std::nano> duration{};
};

template <typename T>
struct DecompressionResults {
    std::vector<T> decompressedData{};
    std::chrono::duration<long, std::nano> duration{};
};

struct CompressionDecompressionResults {
    std::chrono::time_point<std::chrono::system_clock> timestamp{};
    std::string system{};
    std::string dataName{};
    int compressionLevel{};
    int bitsTruncated{};
    int errorBoundMode{};
    int algo{};
    int ialgo{};
    float absErrorBound{};
    int trial{};
    size_t originalSize{};
    double originalMean{};
    double originalRMS{};
    size_t compressedSize{};
    float compressionRatio{};
    double decompressedMean{};
    double decompressedRMS{};
    double meanCE{};
    double rmsCE{};
    std::chrono::duration<long, std::nano> compressionDuration{};
    std::chrono::duration<long, std::nano> decompressionDuration{};
};

// File read/write ---------------------------------------------------------------------------------------------------

template<typename T>
std::vector<T> fileToVector(const std::string& filename) {
    // Open file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("readData: failed to open file " + filename);
    }

    // Read file into vector
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Return data
    return data;
}

template<typename T>
void vectorToFile(const std::string& filename, const std::vector<T>& data) {
    // Open file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("writeData: failed to open file " + filename);
    }

    // Write data to file
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// Data generation ---------------------------------------------------------------------------------------------------

template <typename Distribution>
std::vector<float> generateRandomData(Distribution distribution, int seed, int dataSize) {
    // Allocate Data
    std::vector<float> data(dataSize);

    // Seed generator with provided seed
    std::mt19937 generator;
    generator.seed(seed);

    // Generate data
    for (int i{0}; i < dataSize; i++) {
        data[i] = distribution(generator);
    }

    return data;
}

std::vector<float> generateExponentialFunction(int dataSize, float lambda) {
    // Allocate Data
    std::vector<float> data(dataSize);

    // Generate data
    for (int i{0}; i < dataSize; i++) {
        data[i] = -std::log(1 - static_cast<float>(rand()) / RAND_MAX) / lambda;
    }

    return data;
}

std::vector<float> generateLinearFunction(int dataSize, float slope, float intercept) {
    // Allocate Data
    std::vector<float> data(dataSize);

    // Generate data
    for (int i{0}; i < dataSize; i++) {
        data[i] = slope * i + intercept;
    }

    return data;
}

std::vector<float> generateQuadraticFunction(int dataSize, float a, float b, float c) {
    // Allocate Data
    std::vector<float> data(dataSize);

    // Generate data
    for (int i{0}; i < dataSize; i++) {
        data[i] = a * i * i + b * i + c;
    }

    return data;
}

std::vector<float> genearateSinusoidalFunction(int dataSize, float amplitude, float frequency, float phase) {
    // Allocate Data
    std::vector<float> data(dataSize);

    // Generate data
    for (int i{0}; i < dataSize; i++) {
        data[i] = amplitude * std::sin(2 * M_PI * frequency * i + phase);
    }

    return data;
}

// Other utilities ---------------------------------------------------------------------------------------------------

std::string getHost() {
    // Determine host
    char host[256];
    if (gethostname(host, sizeof(host))) {
        throw std::runtime_error("getHost: failed to get host name");
    }

    return std::string(host);
}

double calculateMean(const std::vector<float>& data) {
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double calculateRMS(const std::vector<float>& data) {
    double mean{calculateMean(data)};
    double sum{0.0};

    for (const float& value : data) {
        sum += std::pow(value - mean, 2);
    }

    return std::sqrt(sum / data.size());
}

// TH1F* vectorToHistogram(const std::vector<float>& data, const std::string& name) {
//     // Get min, max values
//     float basketMin{*std::min_element(data.begin(), data.end())};
//     float basketMax{*std::max_element(data.begin(), data.end())};

//     // Create histogram
//     TH1F* h{new TH1F(name.c_str(), name.c_str(), data.size(), basketMin, basketMax)};

//     // Fill histogram
//     for (const float& value : data) {
//         h->Fill(value);
//     }

//     return h;
// }

#endif