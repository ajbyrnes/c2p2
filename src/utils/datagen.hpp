// Functins for generating dummy data for SZ3

#pragma once

#include <vector>
#include <functional>


std::vector<float> constant1D(size_t n, float value);
std::vector<float> constant2D(size_t n, size_t m, float value); // returns flattened 2D
std::vector<float> constant3D(size_t n, size_t m, size_t k, float value); // returns flattened 3D
std::vector<float> data1D(size_t n, const std::function<float(size_t)>& func);
std::vector<float> data2D(size_t n, size_t m, const std::function<float(size_t, size_t)>& func); // returns flattened 2D
std::vector<float> data3D(size_t n, size_t m, size_t k, const std::function<float(size_t, size_t, size_t)>& func); // returns flattened 3D

float indexToCoord(size_t index, size_t gridSize);