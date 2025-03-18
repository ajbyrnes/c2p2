CXX = g++-13
# CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter
CXXFLAGS = -std=c++20 -pedantic -Wno-unused-parameter

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/SZ3
SZ_INCLUDE_FLAGS = -I$(SZ_DIR)/include 
SZ_LINK_FLAGS = -L$(SZ_DIR)/bin -lzstd

ROOT_FLAGS = $(shell root-config --cflags --libs)

EXECS = corpus_zlib random_zlib random_sz func_sz

all: $(EXECS)

corpus_zlib: corpus_zlib.cpp lib_zlib.hpp lib_utils.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(ZLIB_FLAGS) 

random_zlib: random_zlib.cpp lib_zlib.hpp lib_utils.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(ZLIB_FLAGS)

random_sz: random_sz.cpp lib_sz.hpp lib_utils.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(SZ_INCLUDE_FLAGS) $(SZ_LINK_FLAGS)

func_sz: func_sz.cpp lib_sz.hpp lib_utils.hpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(SZ_INCLUDE_FLAGS) $(SZ_LINK_FLAGS)

clean:
	rm -f $(EXECS) *.o