#include "waveform.h"
#include <algorithm>
#include <iomanip>
#include <math.h>    

int gcd(int a, int b) { //Greatest Common Divisor
  if(b == 0)
    return a;
  return gcd(b, a % b);
}

//digits is the number of times we printing it
// May not need 3rd parameter binary!!


std::string decipher(std::string signal, int digits, bool binary) { //Translates signal into wave
	std::string final = "";
  if(binary) { //Prints out a waveform
    if(signal == "0" || signal == "1") {
      for(int i=0; i<digits; i++) {
        if(signal == "0") { //Can make a function to decipher
          final += "_";
        } else if(signal == "1") {
          final += "‾";
        }
      }
    } else if (signal == "x") { //Should be only x
      final = "?";
      for(int i=0; i<digits-1; i++) {
        final += " ";
      }
    }
  } else { //Prints out the number
    //Needs to print the signal length - digits
    int signalLength = signal.length();
    int buffer = 0;
    if(digits > signalLength) { //If the number of digits is greater than the number of characters in signal
      buffer = digits - signalLength; //Number of spaces to add
      // std::cout << signal << " buffer dig>sigLen: " << buffer << std::endl;

      //Need to fix
      for (int i=0; i<floor(buffer/2); i++) {
        final += " ";
      }
      final += signal;
      for (int i=0; i<ceil(buffer/2); i++) {
        final += " ";
      }
      if(signalLength % 2 != 0){ //Adds the extra space for odd digits
        final += " ";
      }
    } else { //If digits is less than length of signal
      //Print at least two .. or maybe one .
      //Must guarantee at least 4 digits have to change the waveform property to allow minimum 4 digits if a signalLength >= 4
      //If signalLength >= interval and interval < 4 set digits to 4
        //Do this in while parsing
      buffer = digits - 2; //Buffer is the signal length minus .. and the |. This is the number of characters to print from signal
      final += ".."; //Assuming there are at least two available spaces
      final += signal.substr(signalLength - buffer); //Add the last signalLength - 2 characters
      //Add specific length cases to have .. vs .
      //Remove initial | by getting rid of the | at top
    }\
  }
	return final;
}

template <class T>
int numDigits(T number) {
	int digits = 0;
	if (number < 0) digits = 1; // remove this line if '-' counts as a digit
	while (number) {
    number /= 10;
    digits++;
	}
	return digits+1; //To give one extra space for nice printing
}


void Waveform::add_monitor(std::string var) { //Simply adds a waveSignal without any attributes to Waveform
  //User must remember to add a name manually and respective values
  Wave wave;
  wave.name = var;
  waveSignal.push_back(wave);
}

void Waveform::del_monitor(std::string var) { //Simply deletes the Wave from WaveSignal vector
  for(std::vector<Wave>::iterator wave = waveSignal.begin(); wave != waveSignal.end(); ++wave) {
    if(wave->name == var) {
      waveSignal.erase(wave);
    }
  }
  //Updates smallest interval
  std::vector<int> intervalVector;
  for(std::vector<Wave>::iterator wave = waveSignal.begin(); wave != waveSignal.end(); ++wave) {
    for(std::vector<std::pair<int, std::string>>::iterator tsIter = (wave->timeSignal).begin(); tsIter != (wave->timeSignal).end()-1; ++tsIter) {
      intervalVector.push_back(tsIter->first);
    }
  }
  int prev = 0;
  int gcd1 = INT_MAX;
  for(std::vector<int>::iterator intIterator = intervalVector.begin(); intIterator != intervalVector.end()-1; ++intIterator) {
    int tempGCD = gcd(gcd(prev, *intIterator), *(intIterator+1));
    if((intIterator != intervalVector.begin()) && tempGCD < gcd1) {
      gcd1 = tempGCD;
    }
    prev = *intIterator;
  }
  smallestInterval = gcd1;
}

void Waveform::show(int fromTime, int toTime) { //Shows all waves from fromTime to toTime
  if(toTime > totalTime) {
    std::cout << "Waveform show Error: Range must be within bounds." << std::endl;
  } else {
    int digits = numDigits(toTime);
    if((digits < 4) && (longestSignalDigits > digits)) {
      digits = 4; //Set for better printing
    }
    //Can set min

    std::cout << std::setw(longestSignalName+8) << std::left << "Clock: ";
    for(int clock=fromTime; clock<=toTime; clock+=smallestInterval) { //Sets the appropriate width for increased aesthetic
      std::cout << std::setw(numDigits(toTime)) << std::left << clock;
    }
    std::cout << "\n" << std::endl;

    for(auto &wave : waveSignal) { //Prints out the wave
      int currClock = 0;
      std::cout << "Wave: " << std::setw(longestSignalName+2) << std::left << wave.name;
      auto waveSignal = wave.timeSignal.begin();
      std::string prevSignal = "";
      int prevSignalTime = 0;
      while(waveSignal != wave.timeSignal.end()) { //Make sure to check that we print to maximum clock for all
        if(waveSignal->first > toTime || currClock > toTime) {
          break;
        }
        if((waveSignal != wave.timeSignal.begin()) && (waveSignal->first >= fromTime)) {
          if(waveSignal->first == currClock) { //If the time is same as current clock
            if(waveSignal->second != prevSignal) {
              if(wave.binary && (prevSignal == "0" && (waveSignal->second) == "1")) {
                std::cout << "/‾" << decipher(waveSignal->second, digits - 2, wave.binary);
              } else if(wave.binary && (prevSignal == "1" && (waveSignal->second) == "0")) {
                std::cout << "\\_" << decipher(waveSignal->second, digits - 2, wave.binary);
              } else if(wave.binary) { //Handles x case and others if caught
                std::cout << decipher(waveSignal->second, digits, wave.binary);
              } else{
                std::cout << "|" << decipher(waveSignal->second, digits - 1, wave.binary);
              }
            } else { //Same as previous signal
              if(wave.binary) {
                std::cout << decipher(waveSignal->second, digits, wave.binary);
              } else {
                std::cout << "|" << decipher(waveSignal->second, digits - 1, wave.binary);
              }
            }
            prevSignal = waveSignal->second;
            prevSignalTime = currClock;
            waveSignal++;
          } else if(waveSignal->first > currClock) { //If the signals time is further than current clock print previous signal and dont advance to next signal
            if(wave.binary) {
              std::cout << decipher(prevSignal, digits, wave.binary);
            } else {
              std::cout << "|" << decipher(prevSignal, digits - 1, wave.binary);
            }
          }
        } else { //If it is the first signal, advance to next signal
          std::cout << decipher(waveSignal->second, digits, wave.binary);
          prevSignal = waveSignal->second;
          prevSignalTime = currClock;
          waveSignal++;
        }
        currClock += smallestInterval;
      }
      if(currClock <= toTime || prevSignalTime < toTime) { //Check to see if we need to extend prevSignal until toTime is reached
        int times_to_repeat = (toTime - prevSignalTime)/smallestInterval;
        for(int i=0; i<times_to_repeat; i++) {
          if(wave.binary) {
            std::cout << decipher(prevSignal, digits, wave.binary);
          } else {
            std::cout << "|" << decipher(prevSignal, digits - 1, wave.binary);
          }        
        }
      }

      std::cout << "\n" << std::endl;
    }
  } 
}

void Waveform::update(std::string var, int time, std::string value) { //Updates the Waveform, if not there, then add to Waveform, updates smallest interval
  bool inWave = false;
  bool timeInWave = false;
  std::pair<int, std::string> timeSignalPair;
  timeSignalPair.first = time;
  timeSignalPair.second = value;
  for(std::vector<Wave>::iterator wave = waveSignal.begin(); wave != waveSignal.end(); ++wave) {
    (wave->timeSignal).push_back(timeSignalPair);
    if(wave->name == var) {
      inWave = true;
      for(auto &ts : wave->timeSignal) { //For each timesignal pair
        if(ts.first == time) {
          timeInWave = true;
          ts.second = value;
          break;
        } else if ((ts.first > time) && !timeInWave) { //Time is not in the wave, but the wave does exist so we need to sort timeSignal by time after adding
          sort((wave->timeSignal).begin(), (wave->timeSignal).end()); //Need to make sure this works
          //Calculate new smallest interval
          int prev = 0;
          int gcd1 = smallestInterval;
          for(std::vector<std::pair<int, std::string>>::iterator tsIter = (wave->timeSignal).begin(); tsIter != (wave->timeSignal).end()-1; ++tsIter) {
            int tempGCD = gcd(gcd(prev, tsIter->first), (tsIter+1)->first);
            if((tsIter != (wave->timeSignal).begin()) && tempGCD < gcd1) {
              gcd1 = tempGCD;
            }
            prev = tsIter->first;
          }
          smallestInterval = gcd1;
        }
      }
    }
  }
  if(!inWave) { //If the searched var not in waveForm, then make a new wave
    Wave wave;
    wave.name = var;
    std::pair<int, std::string> timeSignal;
    timeSignal.first = time;
    timeSignal.second = value;
    wave.timeSignal.push_back(timeSignal);
    //User needs to manually assign a symbol
    waveSignal.push_back(wave);
  }
}

std::string Waveform::get(std::string var, int time) { //Returns a character ('0', '1' or 'x'). Assumes string var is the name
  std::string result = "?"; //By default if wave does not exist
  for(std::vector<Wave>::iterator wave = waveSignal.begin(); wave != waveSignal.end(); ++wave) {
    if(wave->name == var) {
      for (std::vector<std::pair<int, std::string>>::iterator ts = (wave->timeSignal).begin(); ts != (wave->timeSignal).end(); ++ts) {
        if(ts->first == time) {
          return ts->second;
        } else if(ts->first < time) {
          result = ts->second;
        } else if(ts->first > time) {
          return result;
        }
      }
    }
  }
  return result;
} // returns value
