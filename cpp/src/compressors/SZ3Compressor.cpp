
// /**
//  * @file SZ3Compressor.cpp
//  * @brief Implementation of SZ3Compressor for scientific data compression using SZ3 library.
//  */
// #include "SZ3Compressor.hpp"
// #include <SZ3/api/sz.hpp>

// SZ3::Config makeConfig(std::vector<size_t> dims) {
//     switch (dims.size()) {
//         case 1:
//             return SZ3::Config({dims[0]});
//         case 2:
//             return SZ3::Config({dims[0], dims[1]});
//         case 3:
//             return SZ3::Config({dims[0], dims[1], dims[2]});
//         default:
//             throw std::invalid_argument("Unsupported number of dimensions: " + std::to_string(dims.size()));
//     }
// }

// SZ3Compressor::SZ3Compressor(int algorithm, double errorBound, bool absError) {
//     // Create config object
//     config_ = SZ3::Config();

//     // Set error bound mode
    

//     // Set error bound
//     if (absError) {
//         relError_ = -1; // Set relative error to 0 if absolute error is used
//         absError_ = errorBound;
//     } else {
//         relError_ = errorBound;
//         absError_ = -1; // Set absolute error to 0 if relative error is used
//     }

//     // Set algorithm
//     if (algorithm < 0 || algorithm > 3) {
//         throw std::invalid_argument("Invalid algorithm specified. Must be between 0 and 3.");
//     }

//     algorithm_ = algorithm;
// }

// CompressedData SZ3Compressor::compress(const UncompressedData& data) {
//     SZ3::Config config = makeConfig(data.dims);

//     // config.lossless = false;
//     config.dataType = SZ_FLOAT;
//     config.cmprAlgo = static_cast<SZ3::ALGO>(algorithm_);

//     if (relError_ == -1) {      // Use absolute error bound
//         config.errorBoundMode = SZ3::EB_ABS;
//         config.absErrorBound = absError_;
//     }
//     else {
//         config.errorBoundMode = SZ3::EB_REL; // Use relative error bound
//         config.relErrorBound = relError_;
//     }

//     // Compress
//     size_t cmpSize = 0;
//     char* cmpData{};

//     cmpData = SZ_compress(
//         config,
//         data.data.data(),
//         cmpSize
//     );

//     // std::cout << "[DEBUG] SZ_compress returned cmpData=" << static_cast<void*>(cmpData) << ", cmpSize=" << cmpSize << std::endl;
//     if (!cmpData) {
//         throw std::runtime_error("SZ_compress failed to allocate output buffer");
//     }

//     // std::cout << "[DEBUG] About to allocate compressedData vector of size: " << cmpSize << std::endl;
//     // Move compressed data to CompressedData struct
//     std::vector<uint8_t> compressedData(cmpData, cmpData + cmpSize);

//     // Free the compressed data pointer
//     free(cmpData);

//     return {
//         .data = compressedData,
//         .numFloats = data.data.size(),
//         .dataName = data.dataName,
//         .ogDataFileName = data.fileName,
//         .dims = data.dims
//     };
// }

// DecompressedData SZ3Compressor::decompress(const CompressedData& compressed) {
//     // std::cout << "[DEBUG] Compressed data length: " << compressed.data.size() << std::endl;
//     // std::cout << "[DEBUG] Number of floats to decompress: " << compressed.numFloats << std::endl;

//     // Create config as in compress (using dims from compressed.numFloats)
//     SZ3::Config config = makeConfig(compressed.dims);

//     // config.lossless = false;
//     config.dataType = SZ_FLOAT;
//     config.cmprAlgo = static_cast<SZ3::ALGO>(algorithm_);

//     if (relError_ == -1) {      // Use absolute error bound
//         config.errorBoundMode = SZ3::EB_ABS;
//         config.absErrorBound = absError_;
//     }
//     else {
//         config.errorBoundMode = SZ3::EB_REL; // Use relative error bound
//         config.relErrorBound = relError_;
//     }

//     // Allocate output buffer
//     float* dec_data_p = nullptr;

//     // Call SZ_decompress
//     SZ_decompress(
//         config,
//         reinterpret_cast<const char*>(compressed.data.data()),
//         compressed.data.size(), // Pass compressed buffer size in bytes
//         dec_data_p
//     );
//     // std::cout << "[DEBUG] SZ_decompress returned dec_data_p=" << static_cast<void*>(dec_data_p) << std::endl;
//     if (!dec_data_p) {
//         throw std::runtime_error("SZ_decompress failed to allocate output buffer");
//     }
//     // std::cout << "[DEBUG] About to allocate dec_data vector of size: " << compressed.numFloats << std::endl;
//     std::vector<float> dec_data(dec_data_p, dec_data_p + compressed.numFloats);
//     free(dec_data_p); // Free the decompressed data pointer

//     return {
//         .data = dec_data,
//         .compressor = this->toString(),
//         .dataName = compressed.dataName, // Assuming compressed.dataName is set correctly
//         .ogDataFileName = compressed.ogDataFileName // Assuming compressed.ogDataFileName is set correctly
//     };
// }

// std::map<std::string, std::string> SZ3Compressor::getConfig() const {
//     return {
//         {"relBound", std::format("{:.6f}", relError_)},
//         {"absBound", std::format("{:.6f}", absError_)},
//         {"algorithm", std::to_string(algorithm_)}
//     };
// }

// std::string SZ3Compressor::toString() const {
//     return "SZ3Compressor{" + std::format("rel={:.6f};abs={:.6f};algo={}", relError_, absError_, algorithm_) + "}";
// }