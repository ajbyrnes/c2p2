#include <vector>
#include <functional>
#include <format>
#include <iostream>

#include "../utils/datagen.hpp"

int main() {
    // Example usage of the data generation functions
    size_t n = 12;

    // Use constant1D
    std::vector<float> constantData2D = constant2D(n, n, 5.0f);

    // Print constantData2D
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            std::cout << std::format("{:3} ", constantData2D[n * i + j]);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;
    
    // Use polynomial2D to generate a circle
    // i indexes the y-axis, j indexes the x-axis
    std::vector<float> circularData2D = data2D(n, n, [n](size_t i, size_t j) {
        float x = indexToCoord(j, n);
        float y = indexToCoord(i, n);
        return x * x + y * y;
    });

    // Print circularData2D so that x increases to the right and y increases upwards
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            std::cout << std::format("{:5.2f} ", circularData2D[n * (n - 1 - i) + j]);
        }
        std::cout << std::endl;
    }

    return 0;
}