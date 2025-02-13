CXX = g++-13
CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter

ZLIB_FLAGS = -lz

SZ3_DIR = $(HOME)/SZ3
SZ_FLAGS = -I$(SZ3_DIR)/include


EXECS = test-zlib test-rounding

all: $(EXECS)

test-zlib: test-zlib.cpp compression_lib.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< compression_lib.hpp $(ZLIB_FLAGS)

test-rounding: test-rounding.cpp compression_lib.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< compression_lib.hpp

clean:
	rm -f $(EXECS)