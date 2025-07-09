#pragma once

#include "Compressor.hpp"
#include <vector>
#include <memory>

class SZ3Compressor : public Compressor {
public:
    SZ3Compressor(double relError = 1e-3, int algorithm = 0);

    CompressedData compress(const UncompressedData& data) override;
    std::vector<float> decompress(const CompressedData& compressed) override;

    double getRelError() const;
    void setRelError(double relError);

    int getAlgorithm() const;
    void setAlgorithm(int algorithm);

    std::map<std::string, std::string> getConfig() const override;
    std::string toString() const override;

private:
    double relError_;
    int algorithm_;
};