#include <format>
#include <iostream>
#include <random>

#include "compression_lib.hpp"

int main() {
    // Generate random floats
    std::vector<float> randData(10);
    std::default_random_engine gen(12345);
    std::uniform_real_distribution<float> dis(0.0, 1.0);

    for (float& value : randData) {
        value = dis(gen);
    }

    // Print floats
    for (const float& value : randData) {
        std::cout << std::format("{:.32f}", value) << std::endl;
    }

    // Truncate floats
    for (int bits{1}; bits <= 23; ++bits) {
        std::cout << "Truncating with " << bits << " bits:" << std::endl;
        for (const float& value : randData) {
            std::cout << std::format("{:.32f}", truncateNoRounding(value, bits)) << std::endl;
        }
    }   
}