CXX = g++-13
# CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter
CXXFLAGS = -std=c++20 -pedantic -Wno-unused-parameter

ZLIB_FLAGS = -lz

SZ_DIR = $(HOME)/SZ3
SZ_COMPILE_FLAGS = -I$(SZ_DIR)/include 
SZ_LINK_FLAGS = -L$(SZ_DIR)/bin -lzstd
SZ_FLAGS = $(SZ_COMPILE_FLAGS) $(SZ_LINK_FLAGS)

ROOT_FLAGS = $(shell root-config --cflags --libs)

# EXECS = hello-zlib hello-sz \
# 		test-rounding test-truncation \
# 		test-zlib test-sz

EXECS = test-zlib test-sz

all: $(EXECS)

hello-zlib: hello-zlib.cpp compression_lib.hpp
	$(CXX) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) $(ZLIB_FLAGS) $(SZ_FLAGS)

hello-sz: hello-sz.cpp compression_lib.hpp
	$(CXX) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) $(ZLIB_FLAGS) $(SZ_FLAGS)

test-rounding: test-rounding.cpp compression_lib.hpp
	$(CXX) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) $(ZLIB_FLAGS) $(SZ_FLAGS)

test-truncation: test-truncation.cpp compression_lib.hpp
	$(CXX) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) $(ZLIB_FLAGS) $(SZ_FLAGS)

test-zlib: test-zlib.cpp compression_lib.hpp
	$(CXX) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) $(ZLIB_FLAGS) $(SZ_FLAGS)

test-sz: test-sz.cpp compression_lib.hpp
	$(CXX) -o $@ $^ $(ROOT_FLAGS) $(CXXFLAGS) $(ZLIB_FLAGS) $(SZ_FLAGS)

clean:
	rm -f $(EXECS) *.o