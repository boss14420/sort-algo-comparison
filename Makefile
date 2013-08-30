CC 		= 	gcc
CXX 		= 	g++
#CXXFLAGS	= 	-Wall -std=c++11 -g -O0 -fno-implicit-inline-templates -pg
CXXFLAGS	= 	-Wall -std=c++11 -O3 -march=native
#LIBS 	= 
INCLUDE = -I .

all: test-sort

test-sort: test-sort.cc sort.hpp.gch 
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $@

sort.hpp.gch: sort.hpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE) $< -o $@

clean:
	rm *.gch
