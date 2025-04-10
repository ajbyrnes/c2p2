#ifndef LIB_UTILS_HPP
#define LIB_UTILS_HPP

#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <vector>
#include <unistd.h>

// Data types -----------------------------------------------------------------------------------------------------

using CompressorIn = std::vector<float>;
using CompressorOut = std::vector<uint8_t>;
using DecompressorIn = std::vector<uint8_t>;
using DecompressorOut = std::vector<float>;
using CompressorParams = std::map<std::string, double>;

// Constants -----------------------------------------------------------------------------------------------------

constexpr size_t KB{1'000};
constexpr size_t MB{1'000'000};

// Test functions ---------------------------------------------------------------------------------------------

std::string getTestName(int test) {
    switch (test) {
        case 1: return "UniformRandom";
        case 2: return "Sine";
        case 3: return "Poly";
        case 4: return "NoisySine";
        case 5: return "NoisyPoly";
        default: return "UniformRandom";
    }
}

void parseArguments(int argc, char* argv[], int& numFloats, int& precision,
                    double& min, double& max, double& noise, 
                    int& test, int& iterations) {

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-n") {
            numFloats = std::stod(argv[++i]) * MB / sizeof(float);
        } else if (arg == "-p") {
            precision = std::stoi(argv[++i]);
        } else if (arg == "-min") {
            min = std::stod(argv[++i]);
        } else if (arg == "-max") {
            max = std::stod(argv[++i]);
        } else if (arg == "-noise") {
            noise = std::stod(argv[++i]);
        } else if (arg == "-test") {
            test = std::stoi(argv[++i]);
        } else if (arg == "-i") {
            iterations = std::stoi(argv[++i]);
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Usage: " << argv[0] << " [-n numFloats] [-p precision] [-min min] [-max max] [-noise noise] [-test test] [-iterations iterations]" << std::endl;
            exit(1);
        }
    }

    // Print values
    std::cout << "numFloats: " << numFloats << "\n";
    std::cout << "precision: " << precision << "\n";
    std::cout << "min: " << min << "\n";
    std::cout << "max: " << max << "\n";
    std::cout << "iterations: " << iterations << "\n";
    std::cout << "test: " << test << "\n";
    
    if (test == 4 || test == 5) {
        std::cout << "  noise: " << noise << "\n";
    }
    std::cout << std::endl;
}

std::ofstream openCSV(std::string filename, const CompressorParams& params) {
    // Tag file with timestamp to avoid overwriting
    std::string timestamp{std::format("{}_{}.csv", filename, std::chrono::system_clock::now().time_since_epoch().count())};
    std::ofstream file(timestamp);
    if (!file.is_open()) {
        throw std::runtime_error("openCSV: failed to open file " + filename);
    }

    // Write header
    file << "system,";
    file << "timestamp,";
    file << "name,";
    for (const auto& param : params) {
        file << param.first << ",";
    }
    file << "data size (bytes),";
    file << "min,";
    file << "max,";
    file << "compression_time (ms),";
    file << "decompression_time (ms),";
    file << "compression_ratio,";
    file << "compression_error_avg,";
    file << "compression_error_min,";
    file << "compression_error_max";
    file << std::endl;
    
    return file;
}

// Data generation ---------------------------------------------------------------------------------------------------

CompressorIn generateUniformRandomData(size_t numFloats, float min, float max, int seed) {
    std::uniform_real_distribution<float> distribution(min, max);
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

CompressorIn addNoise(const CompressorIn& data, float noiseLevel) {
    CompressorIn noisyData(data.size());
    std::mt19937 generator(1234);
    std::normal_distribution<float> distribution(-noiseLevel, noiseLevel);
    for (size_t i = 0; i < data.size(); ++i) {
        noisyData[i] = data[i] + distribution(generator);
    }
    return noisyData;
}

CompressorIn generateTestData(int test, int numFloats, float xmin, float xmax, float noise=0) {
    switch(test) {
        case 1: default: // Uniform random data
            return generateUniformRandomData(numFloats, xmin, xmax, 1234);
        case 2: // Sine data
            return generateSineData(numFloats, xmin, xmax);
        case 3: // Polynomial data
            return generatePolynomialData(numFloats, xmin, xmax);
        case 4: // Noisy Sine data
            return addNoise(generateSineData(numFloats, xmin, xmax), noise);
        case 5: // Noisy Polynomial data
            return addNoise(generatePolynomialData(numFloats, xmin, xmax), noise);
    }
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