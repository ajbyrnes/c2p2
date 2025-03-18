CXX = g++-13
# CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Wno-unused-parameter
CXXFLAGS = -std=c++20 -pedantic -Wno-unused-parameter

SZ_DIR = $(HOME)/SZ3
SZ_INCLUDE_FLAGS = -I$(SZ_DIR)/include 
SZ_LINK_FLAGS = -L$(SZ_DIR)/bin -lzstd

EXECS = hello_sz

all: $(EXECS)

hello_sz: hello_sz.cpp
	$(CXX) -o $@ $^ $(CXXFLAGS) $(SZ_INCLUDE_FLAGS) $(SZ_LINK_FLAGS)

clean:
	rm -f $(EXECS) *.o