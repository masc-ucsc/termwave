#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <utility>

#ifndef WAVEFORM_H
#define WAVEFORM_H

// struct TimeSig {
//   int time;
//   char signal;
// };


// struct Wave { //Each Wave will have its symbol name and a vector of associated times
// 	 string symbolName;
//   vector<TimeSig> timeSignal;
// };

class Waveform {
  public:
    std::string symbol;
    std::string name;
    std::vector<std::pair<int, char>> timeSignal; //Contains each wave's signal at each time


    void add_monitor(std::string var);
    void del_monitor(std::string var);
    void show(int fromTime, int toTime);
    // Read a vcd file
    void update(std::string var, int time, int value);
    int get(std::string var, int time); // returns value
};



#endif