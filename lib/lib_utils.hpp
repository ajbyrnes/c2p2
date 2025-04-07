#include <chrono>
#include <format>
#include <random>
#include <vector>
#include <unistd.h>

#ifndef LIB_UTILS_HPP
#define LIB_UTILS_HPP

// Data types -----------------------------------------------------------------------------------------------------

using CompressorIn = std::vector<float>;
using CompressorOut = std::vector<uint8_t>;
using DecompressorIn = std::vector<uint8_t>;
using DecompressorOut = std::vector<float>;
using CompressorParams = std::map<std::string, int>;

// Constants -----------------------------------------------------------------------------------------------------
constexpr size_t KB{1'000};
constexpr size_t MB{1'000'000};

// Data generation ---------------------------------------------------------------------------------------------------

CompressorIn generateRandomData(std::uniform_real_distribution<float>& distribution, int seed, size_t numFloats) {
    std::mt19937 generator(seed);
    CompressorIn data(numFloats);
    for (size_t i = 0; i < numFloats; ++i) {
        data[i] = distribution(generator);
    }
    return data;
}

CompressorIn generateXData(size_t dataSize, double xmin, double xmax) {
    CompressorIn data(dataSize);
    double step = (xmax - xmin) / (dataSize - 1);
    for (size_t i = 0; i < dataSize; ++i) {
        data[i] = xmin + i * step;
    }
    return data;
}

CompressorIn generateSineData(size_t dataSize, double xmin, double xmax) {
    CompressorIn data(dataSize);
    double step = (xmax - xmin) / (dataSize - 1);
    for (size_t i = 0; i < dataSize; ++i) {
        data[i] = std::sin(xmin + i * step);
    }
    return data;
}

CompressorIn generatePolynomialData(size_t dataSize, double xmin, double xmax) {
    CompressorIn data(dataSize);
    double step = (xmax - xmin) / (dataSize - 1);
    for (size_t i = 0; i < dataSize; ++i) {
        double x = xmin + i * step;
        data[i] = std::pow(x, 3) - 2 * std::pow(x, 2) + x + 1;
    }
    return data;
}

CompressorIn addNoise(const CompressorIn& data, double noiseLevel) {
    CompressorIn noisyData(data.size());
    std::mt19937 generator(1234);
    std::normal_distribution<float> distribution(-noiseLevel, noiseLevel);
    for (size_t i = 0; i < data.size(); ++i) {
        noisyData[i] = data[i] + distribution(generator);
    }
    return noisyData;
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