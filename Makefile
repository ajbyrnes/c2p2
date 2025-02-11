CXX = g++-13
CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter

SZ3_DIR = $(HOME)/SZ3
LDFLAGS = -I$(SZ3_DIR)/include

EXECS = sz-example

all: $(EXECS)

sz-example: sz-example.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(EXECS)