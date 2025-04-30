CXX = g++-13
CXXFLAGS = -std=c++20

LIB_FLAGS = -I../lib

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/lib/SZ3/SZ3_install
SZ_INCLUDE = -I$(SZ_DIR)/include
SZ_LIB = -L$(SZ_DIR)/lib -lzstd

ROOT_FLAGS = $(shell root-config --cflags --libs)

TRUNK_SRCS = correctness_TrunkCompressor.cpp
SZ_SRCS = correctness_SZCompressor.cpp
BENCH_SRCS = benchmark.cpp rootToVector.cpp

TRUNK_EXECS = $(TRUNK_SRCS:.cpp=)
SZ_EXECS = $(SZ_SRCS:.cpp=)
BENCH_EXECS = $(BENCH_SRCS:.cpp=)

# all: $(TRUNK_EXECS) $(SZ_EXECS) $(BENCH_EXECS)
all: $(BENCH_EXECS)

# $(TRUNK_EXECS): %: %.cpp lib/utils.hpp lib/TrunkCompressor.hpp
# 	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(ZLIB_FLAGS)

# $(SZ_EXECS): %: %.cpp lib/utils.hpp lib/SZCompressor.hpp
# 	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(SZ_INCLUDE) $(SZ_LIB)

$(BENCH_EXECS): %: %.cpp lib/utils.hpp lib/TrunkCompressor.hpp lib/SZCompressor.hpp lib/CompressorBench.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(ZLIB_FLAGS) $(SZ_INCLUDE) $(SZ_LIB) $(ROOT_FLAGS)

clean:
	rm -f $(TRUNK_EXECS) $(SZ_EXECS) $(BENCH_EXECS)