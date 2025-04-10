#include <iostream>
#include <map>
#include <string>
#include <format>
#include <random>
#include <fstream>

#include "lib/lib_sz.hpp"

int main(int argc, char* argv[]) {
    // Default parameters
    int NUMFLOATS{10 * MB / sizeof(float)};
    double MIN{0.0};
    double MAX{100.0};
    int ITERATIONS{10};
    double NOISE{0.1};
    int TEST{1}; // 1 for uniform random data
    int PRECISION{3}; // 3 digits of precision

    // Parse command line arguments
    parseArguments(argc, argv, NUMFLOATS, PRECISION, MIN, MAX, NOISE, TEST, ITERATIONS);

    // Generate test data
    CompressorIn data{generateTestData(TEST, NUMFLOATS, MIN, MAX, NOISE)};

    // Sweep sz3 parameters
    paramSweepSZ3(data, PRECISION, TEST, ITERATIONS, NOISE);

    return 0;
}
