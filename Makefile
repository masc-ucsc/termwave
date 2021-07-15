CXX = g++
CXXFLAGS = -Wall -std=c++11 

all: Main

Main: Main.o Parser.o
	$(CXX) -g $(CXXFLAGS) -o Main Main.o Parser.o

Main.o: Main.cpp Parser.h
	$(CXX) -g $(CXXFLAGS) -c Main.cpp

Parser.o: Parser.cpp Parser.h
	$(CXX) -g $(CXXFLAGS) -c Parser.cpp

clean:
	rm -rf Main Main.o Parser.o