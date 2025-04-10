CXX = g++-13
CXXFLAGS = -std=c++20 -pedantic -Wno-unused-parameter

LIB_FLAGS = -I../lib

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/SZ3
SZ_INCLUDE = -I$(SZ_DIR)/include
SZ_LIB = -L$(SZ_DIR)/lib -lzstd

ROOT_FLAGS = $(shell root-config --cflags --libs)

ZLIB_SRCS = correctness_zlib.cpp paramSweep_zlibTrunc.cpp 
SZ_SRCS = correctness_sz3.cpp paramSweep_sz3.cpp 
ROOT_SRCS = graph.cpp

ZLIB_EXECS = $(ZLIB_SRCS:.cpp=)
SZ_EXECS = $(SZ_SRCS:.cpp=)
ROOT_EXECS = $(ROOT_SRCS:.cpp=)

all: $(ZLIB_EXECS) $(SZ_EXECS) $(ROOT_EXECS)

$(ZLIB_EXECS): %: %.cpp lib/lib_utils.hpp lib/lib_zlib.hpp lib/CompressorBenchmark.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(ZLIB_FLAGS)

$(SZ_EXECS): %: %.cpp lib/lib_utils.hpp lib/lib_sz.hpp lib/CompressorBenchmark.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(SZ_INCLUDE) $(SZ_LIB)

$(ROOT_EXECS): %: %.cpp lib/lib_utils.hpp lib/lib_zlib.hpp lib/CompressorBenchmark.hpp
	$(CXX) -o $@ $^ $(LIB_FLAGS) $(ZLIB_FLAGS) $(SZ_INCLUDE) $(SZ_LIB) $(ROOT_FLAGS)

clean:
	rm -f $(ZLIB_EXECS) $(SZ_EXECS)