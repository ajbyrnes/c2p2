CXX = g++-13
CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter

EXECS = zlib-example single_normal many_normals truncation

all: $(EXECS)

zlib-example: zlib-example.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lz

truncation: truncation.cpp utils.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lz

single_normal: single_normal.cpp utils.hpp
	$(CXX) -o $@ $< -lz `root-config --cflags --libs` $(CXXFLAGS) 

many_normals: many_normals.cpp utils.hpp
	$(CXX) -o $@ $< -lz `root-config --cflags --libs` $(CXXFLAGS)



clean:
	rm -f $(EXECS)