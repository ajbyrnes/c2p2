CXX = g++-13
# CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter
CXXFLAGS = -std=c++20 -pedantic -Wno-unused-parameter

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/SZ3
SZ_COMPILE_FLAGS = -I$(SZ_DIR)/include 
SZ_LINK_FLAGS = -L$(SZ_DIR)/bin -lzstd
SZ_FLAGS = $(SZ_COMPILE_FLAGS) $(SZ_LINK_FLAGS)

ROOT_FLAGS = $(shell root-config --cflags --libs)

LIBS_HDRS = lib_zlib.hpp lib_test.hpp
LIBS_SRCS = lib_zlib.cpp lib_test.cpp
LIBS_OBJS = $(LIBS_SRCS:.cpp=.o)

EXECS = corpus_zlib

all: $(EXECS)

lib_zlib.o: lib_zlib.cpp lib_zlib.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

lib_test.o: lib_test.cpp lib_test.hpp lib_zlib.hpp
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(ROOT_FLAGS) 

corpus_zlib: corpus_zlib.cpp lib_zlib.o lib_test.o
	$(CXX) -o $@ $^ $(ROOT_FLAGS)  $(CXXFLAGS) $(ZLIB_FLAGS)

clean:
	rm -f $(EXECS) *.o