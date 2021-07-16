#include "Waveform.h"
#include <algorithm>
#include <iomanip>

int gcd(int a, int b) { //Greatest Common Divisor
   if (b == 0)
   return a;
   return gcd(b, a % b);
}

std::string decipher(char signal, int digits) { //Translates signal into wave
	std::string final = "";
	if(signal == '0' || signal == '1') {
		for(int i=0; i<digits; i++) {
			if(signal == '0') { //Can make a function to decipher
				final += "_";
			} else if(signal == '1') {
				final += "‾";
			}
		}
	} else {
		final = "?";
		for(int i=0; i<digits-1; i++) {
			final += " ";
		}
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
    for(std::vector<std::pair<int, char>>::iterator tsIter = (wave->timeSignal).begin(); tsIter != (wave->timeSignal).end()-1; ++tsIter) {
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
    std::cout << std::setw(longestSignalName+8) << std::left << "Clock: ";
    for(int clock=fromTime; clock<=toTime; clock+=smallestInterval) { //Sets the appropriate width for increased aesthetic
      std::cout << std::setw(numDigits(toTime)) << std::left << clock;
    }
    std::cout << "\n" << std::endl;

    for (auto &wave : waveSignal) { //Prints out the wave
      int currClock = 0;
      //longest_signal_name_
      std::cout << "Wave: " << std::setw(longestSignalName+2) << std::left << wave.name;
      auto waveSignal = wave.timeSignal.begin();
      char prevSignal = 'b';
      int prevSignalTime = 0;
      while(waveSignal != wave.timeSignal.end()) { //Make sure to check that we print to maximum clock for all
        if(waveSignal->first > toTime || currClock > toTime) {
          break;
        }
        if((waveSignal != wave.timeSignal.begin()) && (waveSignal->first >= fromTime)) {
          if(waveSignal->first == currClock) { //If the time is same as current clock
            if(waveSignal->second != prevSignal) {
              if(prevSignal == '0' && (waveSignal->second) == '1') { //TODO Make this into a function to print number of digits to
                std::cout << "/‾" << decipher(waveSignal->second, digits - 2);
              } else if(prevSignal == '1' && (waveSignal->second) == '0') {
                std::cout << "\\_" << decipher(waveSignal->second, digits - 2);
              } else { //Handles x case and other if caught
                std::cout << decipher(waveSignal->second, digits);
              }
            } else { //Same as previous signal
              std::cout << decipher(waveSignal->second, digits);
            }
            prevSignal = waveSignal->second;
            prevSignalTime = currClock;
            waveSignal++;
          } else if(waveSignal->first > currClock) { //If the signals time is further than current clock print previous signal and dont advance to next signal
            std::cout << decipher(prevSignal, digits);
          } 
        } else { //If it is the first signal, advance to next signal
          std::cout << decipher(waveSignal->second, digits);
          prevSignal = waveSignal->second;
          prevSignalTime = currClock;
          waveSignal++;
        }
        currClock += smallestInterval;
      }
      if(currClock <= toTime || prevSignalTime < toTime) { //Check to see if we need to extend prevSignal until toTime is reached
        int times_to_repeat = (toTime - prevSignalTime)/smallestInterval;
        for(int i=0; i<times_to_repeat; i++) {
          std::cout << decipher(prevSignal, digits);
        }
      }

      std::cout << "\n" << std::endl;
    }
  } 
}

void Waveform::update(std::string var, int time, char value) { //Updates the Waveform, if not there, then add to Waveform, updates smallest interval
  bool inWave = false;
  bool timeInWave = false;
  std::pair<int, char> timeSignalPair;
  timeSignalPair.first = time;
  timeSignalPair.second = value;
  for(std::vector<Wave>::iterator wave = waveSignal.begin(); wave != waveSignal.end(); ++wave) {
    (wave->timeSignal).push_back(timeSignalPair);
    if(wave->name == var) {
      inWave = true;
      for(auto &ts : wave->timeSignal) {
        if(ts.first == time) {
          timeInWave = true;
          ts.second = value;
          break;
        } else if ((ts.first > time) && !timeInWave) { //Time is not in the wave, but the wave does exist so we need to sort timeSignal by time after adding
          sort((wave->timeSignal).begin(), (wave->timeSignal).end()); //Need to make sure this works
          //Calculate new smallest interval
          int prev = 0;
          int gcd1 = smallestInterval;
          for(std::vector<std::pair<int, char>>::iterator tsIter = (wave->timeSignal).begin(); tsIter != (wave->timeSignal).end()-1; ++tsIter) {
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
    std::pair<int, char> timeSignal;
    timeSignal.first = time;
    timeSignal.second = value;
    wave.timeSignal.push_back(timeSignal);
    //User needs to manually assign a symbol
    waveSignal.push_back(wave);
  }
}

char Waveform::get(std::string var, int time) { //Returns a character ('0', '1' or 'x'). Assumes string var is the symbol ex. !, $, %
  char result = '?'; //By default if wave does not exist
  for(std::vector<Wave>::iterator wave = waveSignal.begin(); wave != waveSignal.end(); ++wave) {
    if(wave->name == var) {
      for (std::vector<std::pair<int, char>>::iterator ts = (wave->timeSignal).begin(); ts != (wave->timeSignal).end(); ++ts) {
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
