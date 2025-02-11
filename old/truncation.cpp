
#include <bitset>
#include <iostream>
#include <format>

#include "utils.hpp"

constexpr int MANTISSA{23};

int main() {
    // float x{0.3289165609374};
    // float x{0.3};

    // for (int i{0}; i < MANTISSA; ++i) {
    //     std::cout << std::format("Truncated {:2} bits: {:.20f}\n", i, truncateFloat(x, i));
    // }

    int x{107};
    for (int i{0}; i < 8; ++i) {
        std::cout << std::format("Truncated {:2} bits: {:3}", i, truncateInt(x, i));
        std::cout << std::format(" Binary: ") << std::bitset<8>(truncateInt(x, i)) << '\n';
    }
}