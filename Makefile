CXX = g++-13
CXXFLAGS = -std=c++20 -pedantic -Wno-unused-parameter

LIB_FLAGS = -I../lib

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/SZ3
SZ_INCLUDE = -I$(SZ_DIR)/include
SZ_LIB = -L$(SZ_DIR)/lib -lzstd

ROOT_FLAGS = $(shell root-config --cflags --libs)

ZLIB_SRCS = paramSweep_zlibTrunc_UniformRandom.cpp

SZ_SRCS = paramSweep_sz3_UniformRandom.cpp

ZLIB_EXECS = $(ZLIB_SRCS:.cpp=)
SZ_EXECS = $(SZ_SRCS:.cpp=)

# all: $(ZLIB_EXECS) $(SZ_EXECS)

all: $(SZ_EXECS)

$(ZLIB_EXECS): %: %.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(ZLIB_FLAGS)

$(SZ_EXECS): %: %.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIB_FLAGS) $(SZ_INCLUDE) $(SZ_LIB)

clean:
	rm -f $(ZLIB_EXECS) $(SZ_EXECS)