CXX = g++-13
CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter

EXECS = zlib-example

all: $(EXECS)

zlib-example: zlib-example.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lz

clean:
	rm -f $(EXECS)