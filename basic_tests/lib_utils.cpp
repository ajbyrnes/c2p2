#include <unistd.h>

#include "lib_utils.hpp"

std::string getHost() {
    // Determine host
    char host[256];
    if (gethostname(host, sizeof(host))) {
        throw std::runtime_error("getHost: failed to get host name");
    }

    return std::string(host);
}

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