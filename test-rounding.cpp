#include <bitset>
#include <format>
#include <iostream>
#include <random>

#include "compression_lib.hpp"

int main() {
    // Generate random floats
    for (int i{0}; i < 10; ++i) {
        std::mt19937 gen(12345);
        std::uniform_real_distribution<float> dis(-1.0, 1.0);

        float randomFloat{dis(gen)};
        std::cout << std::format("random float: {:.32f}", randomFloat) << std::endl;
    
        // Truncate float
        for (int i{1}; i <= 4; ++i) {
            int bits{2 ** i};

            float roundedTruncated{truncate(randomFloat, bits)};
            float unroundedTruncated{truncate(randomFloat, bits, false)};
            
            std::cout << std::format("truncated {:2} bits: {:.32f} ", bits, roundedTruncated) << std::endl;
            std::cout << std::format("truncated {:2} bits: {:.32f} ", bits, unroundedTruncated) << std::endl;
        }
    }
}