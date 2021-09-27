#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <utility>
#include <climits>

#ifndef WAVEFORM_H
#define WAVEFORM_H


std::string decipher(char signal, int digits);

struct Wave { //Each independent wave
  std::string symbol;
  std::string name;
  bool binary;
  std::vector<std::pair<int, std::string>> timeSignal; //Contains each wave's signal at each time
};

class Waveform { //Holds all the signals as a Vector of Waves
  public:
    std::vector<Wave> waveSignal;
    int smallestInterval;
    long unsigned int longestSignalName;
    int totalTime;

    void add_monitor(std::string var);
    void del_monitor(std::string var);
    void show(int fromTime, int toTime);
    void update(std::string var, int time, std::string value);
    std::string get(std::string var, int time);
};

#endif