// SZ3 compression ------------------------------------------------------------------------------------------------
// Modified from https://github.com/physnerds/ldrd_compressions/blob/feature/tree/tests/mytest.cxx

std::vector<uint8_t> szCompress(std::vector<float> data, 
    SZ3::EB errorBoundMode=SZ3::EB_ABS,
    SZ3::ALGO algo=SZ3::ALGO_INTERP_LORENZO,
    SZ3::INTERP_ALGO ialgo=SZ3::INTERP_ALGO_LINEAR,
    float absErrorBound=1E-3) {

std::cout << "Compressing data..." << std::endl;
// Configuration
SZ3::Config conf({data.size() * sizeof(float)});
conf.errorBoundMode = errorBoundMode;
conf.cmprAlgo = algo;
conf.interpAlgo = ialgo;
conf.absErrorBound = absErrorBound;

// Compress
size_t compressedSize;
char* compressedData{SZ_compress(conf, data.data(), compressedSize)};

// Return compressed data
return std::vector<uint8_t>(compressedData, compressedData + compressedSize);
}

std::vector<float> szDecompress(const std::vector<uint8_t>& compressedData, const size_t& uncompressedSize) {
std::cout << "Decompressing data..." << std::endl;
// Configuration
SZ3::Config conf({uncompressedSize});

// Convert compressed data to char*
const char* compressedDataPtr{reinterpret_cast<const char*>(compressedData.data())};

// Allocate memory for decompressed data
float* decompressedDataPtr{};

SZ_decompress(conf, compressedDataPtr, compressedData.size(), decompressedDataPtr);

return std::vector<float>(decompressedDataPtr, decompressedDataPtr + uncompressedSize);
}