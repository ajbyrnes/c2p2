CXX = g++-13
# CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter
CXXFLAGS = -std=c++20 -pedantic -Wno-unused-parameter

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/SZ3
SZ_INCLUDE_FLAGS = -I$(SZ_DIR)/include 
SZ_LINK_FLAGS = -L$(SZ_DIR)/lib

ROOT_FLAGS = $(shell root-config --cflags --libs)

LIBS_HDRS = lib_zlib.hpp lib_utils.hpp
LIBS_SRCS = lib_zlib.cpp lib_utils.cpp
LIBS_OBJS = $(LIBS_SRCS:.cpp=.o)

EXECS = corpus_zlib random_zlib random_sz

all: $(EXECS)

lib_utils.o: lib_utils.cpp lib_utils.hpp lib_zlib.hpp
	$(CXX) $(ROOT_FLAGS) $(CXXFLAGS) -c -o $@ $< 

lib_zlib.o: lib_zlib.cpp lib_zlib.hpp lib_utils.hpp
	$(CXX) $(ROOT_FLAGS) $(CXXFLAGS) -c -o $@ $< 

lib_sz.o: lib_sz.cpp lib_sz.hpp lib_utils.hpp
	$(CXX) $(SZ_INCLUDE_FLAGS) $(ROOT_FLAGS) $(CXXFLAGS) -c -o $@ $< 

corpus_zlib: corpus_zlib.cpp lib_zlib.o lib_utils.o
	$(CXX) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) $(ZLIB_FLAGS)

random_zlib: random_zlib.cpp lib_zlib.o lib_utils.o
	$(CXX) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) $(ZLIB_FLAGS)

random_sz: random_sz.cpp lib_sz.o lib_utils.o
	$(CXX) $(SZ_INCLUDE_FLAGS) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) 

clean:
	rm -f $(EXECS) $(LIBS_OBJS)