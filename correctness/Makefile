CXX = g++-13
CXXFLAGS = -std=c++20

LIB_DIR = ../lib
LIB_FLAGS = -I../lib

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/lib/SZ3/SZ3_install
SZ_INCLUDE = -I$(SZ_DIR)/include
SZ_LIB = -L$(SZ_DIR)/lib -lzstd

ROOT_FLAGS = $(shell root-config --cflags --libs)

SRC = correctness_TrunkCompressor.cpp \
		correctness_SZCompressor.cpp \
		correctness_SZZlibCompressor.cpp

EXECS = correctness_TrunkCompressor \
		correctness_SZCompressor \
		correctness_SZZlibCompressor

all: $(EXECS)

correctness_TrunkCompressor: %: %.cpp ${LIB_DIR}/utils.hpp ${LIB_DIR}/TrunkCompressor.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(ZLIB_FLAGS) $(ROOT_FLAGS)

correctness_SZCompressor: %: %.cpp ${LIB_DIR}/utils.hpp ${LIB_DIR}/SZCompressor.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(SZ_INCLUDE) $(SZ_LIB) $(ROOT_FLAGS)

correctness_SZZlibCompressor: %: %.cpp ${LIB_DIR}/utils.hpp ${LIB_DIR}/SZZlibCompressor.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(ZLIB_FLAGS) $(SZ_INCLUDE) $(SZ_LIB) $(ROOT_FLAGS)

clean:
	rm -f $(EXECS)