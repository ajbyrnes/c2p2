// Implementation of data generation functions for SZ3

#include "datagen.hpp"
#include <vector>
#include <functional>

std::vector<float> constant1D(size_t n, float value) {
    std::vector<float> data(n, value);
    return data;
}


std::vector<float> constant2D(size_t n, size_t m, float value) {
    std::vector<float> data(n * m, value);
    return data;
}


std::vector<float> constant3D(size_t n, size_t m, size_t k, float value) {
    std::vector<float> data(n * m * k, value);
    return data;
}

std::vector<float> data1D(size_t n, const std::function<float(size_t)>& func) {
    std::vector<float> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = func(i);
    }
    return data;
}


std::vector<float> data2D(size_t n, size_t m, const std::function<float(size_t, size_t)>& func) {
    std::vector<float> data(n * m);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            data[i * m + j] = func(i, j);
        }
    }
    return data;
}


std::vector<float> data3D(size_t n, size_t m, size_t k, const std::function<float(size_t, size_t, size_t)>& func) {
    std::vector<float> data(n * m * k);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < m; ++j) {
            for (size_t l = 0; l < k; ++l) {
                data[(i * m + j) * k + l] = func(i, j, l);
            }
        }
    }
    return data;
}

float indexToCoord(size_t index, size_t gridSize) {
    return static_cast<float>(index) - static_cast<float>(gridSize) / 2.0f;
}