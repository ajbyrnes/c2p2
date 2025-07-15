#pragma once

#include "Compressor.hpp"
#include <vector>
#include <memory>

class SZ3Compressor : public Compressor {
public:
    SZ3Compressor(int algorithm = 0, double errorBound = 1e-5, bool absError = false);

    CompressedData compress(const UncompressedData& data) override;
    std::vector<float> decompress(const CompressedData& compressed) override;

    double getRelError() const;
    void setRelError(double relError);

    double getAbsError() const;
    void setAbsError(double absError);

    int getAlgorithm() const;
    void setAlgorithm(int algorithm);

    std::map<std::string, std::string> getConfig() const override;
    std::string toString() const override;

private:
    double absError_;
    double relError_;
    int algorithm_;
};