#include "lib_test.hpp"
#include "lib_zlib.hpp"

// Compression -----------------------------------------------------------------------------------------------------

CompressionResults timedZlibCompress(std::vector<uint8_t> data, const int& level=Z_DEFAULT_COMPRESSION) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{zlibCompress<uint8_t>(data, level)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {compressedData, duration};
}

CompressionResults timedZlibTruncateCompress(std::vector<float> data, const int& bits, const int& level) {
    // Start timer
    std::chrono::high_resolution_clock::time_point start{std::chrono::high_resolution_clock::now()};

    // Compress data
    std::vector<uint8_t> compressedData{zlibTruncateCompress(data, bits, level)};

    // End timer
    std::chrono::high_resolution_clock::time_point end{std::chrono::high_resolution_clock::now()};

    // Report results
    std::chrono::duration<long, std::nano> duration{std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)};
    return {compressedData, duration};      // This is needlessly pythonic, should be using pass-by-ref instead
}

// Read/write functions --------------------------------------------------------------------------------------------

std::vector<uint8_t> charFileToVector(const std::string& filename) {
    // Open file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("readData: failed to open file " + filename);
    }

    // Read file into vector
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Return data
    return data;
}

void vectorToCharFile(const std::string& filename, const std::vector<uint8_t>& data) {
    // Open file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("writeData: failed to open file " + filename);
    }

    // Write data to file
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}

// Data generation -------------------------------------------------------------------------------------------------

// Other utilities -------------------------------------------------------------------------------------------------

TH1F* vectorToHistogram(const std::vector<float>& data, const std::string& name) {
    // Get min, max values
    float basketMin{*std::min_element(data.begin(), data.end())};
    float basketMax{*std::max_element(data.begin(), data.end())};

    // Create histogram
    TH1F* h{new TH1F(name.c_str(), name.c_str(), data.size(), basketMin, basketMax)};

    // Fill histogram
    for (const float& value : data) {
        h->Fill(value);
    }

    return h;
}