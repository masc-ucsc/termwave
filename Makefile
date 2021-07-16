CXX = g++
CXXFLAGS = -Wall -std=c++11 

all: Main

Main: Main.o Parser.o Waveform.o
	$(CXX) -g $(CXXFLAGS) -o Main Main.o Parser.o Waveform.o

Main.o: Main.cpp Parser.h
	$(CXX) -g $(CXXFLAGS) -c Main.cpp

Parser.o: Parser.cpp Parser.h
	$(CXX) -g $(CXXFLAGS) -c Parser.cpp

Waveform.o: Waveform.cpp Waveform.h
	$(CXX) -g $(CXXFLAGS) -c Waveform.cpp

clean:
	rm -rf Main Main.o Parser.o Waveform.o