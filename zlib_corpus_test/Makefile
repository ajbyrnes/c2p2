CXX = g++-13
CXXFLAGS = -std=c++20 -pedantic -Wno-unused-parameter

LIB_DIR = ../lib
LIB_INCLUDE = -I$(LIB_DIR)
LIB_ZLIB = $(LIB_DIR)/lib_zlib.hpp
LIB_SZ = $(LIB_DIR)/lib_sz.hpp
LIB_UTILS = $(LIB_DIR)/lib_utils.hpp

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/SZ3
SZ_INCLUDE_FLAGS = -I$(SZ_DIR)/include 
SZ_LINK_FLAGS = -L$(SZ_DIR)/bin -lzstd

EXECS = corpus_zlib

all: $(EXECS)

corpus_zlib: corpus_zlib.cpp $(LIB_ZLIB) $(LIB_UTILS)
	$(CXX) -o $@ $< $(CXXFLAGS) $(ZLIB_FLAGS) $(LIB_INCLUDE)

clean:
	rm -f $(EXECS) *.o