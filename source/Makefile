CXX = g++
CXXFLAGS = -Wall -std=c++11 

all: main

main: main.o parser.o waveform.o
	$(CXX) -g $(CXXFLAGS) -o main main.o parser.o waveform.o

main.o: main.cpp parser.h
	$(CXX) -g $(CXXFLAGS) -c main.cpp

parser.o: parser.cpp parser.h
	$(CXX) -g $(CXXFLAGS) -c parser.cpp

waveform.o: waveform.cpp waveform.h
	$(CXX) -g $(CXXFLAGS) -c waveform.cpp

clean:
	rm -rf main main.o parser.o waveform.o