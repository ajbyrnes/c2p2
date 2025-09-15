#include <format>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>

#include <nlohmann/json.hpp>

#include "../utils/utils.hpp"
#include "../utils/root.hpp"
#include "../utils/cli.hpp"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    std::string compressor = "BitTruncation";
    float compressionRatio = 10;
    int mantissaBits = 8;
    int chunkSize = 1024;

    // Create JSON object
    json j1;
    j1["compressor"] = compressor;
    j1["compressionRatio"] = compressionRatio;
    j1["mantissaBits"] = mantissaBits;
    j1["chunkSize"] = chunkSize;

    std::ofstream o("results.json");
    o << std::setw(4) << j1 << std::endl;
    o.close();

    // Read JSON
    std::string json_filename = "results.json";
    std::ifstream f(json_filename);

    json data = json::parse(f);

    std::cout << std::setw(4) << data << std::endl;

    return 0;
}