#include "parser.h"
#include <algorithm>
#include <iomanip>
#include <climits>


/**
 * \brief Splits the given string by space in a vector
 * \param str string to be splitted
 * \return A vector containing the the splitted line_
 */
std::vector<std::string> SplitBySpace(std::string str) {
	std::string buf;
	std::stringstream ss(str);

	std::vector<std::string> tokens;

	while (ss >> buf)
		tokens.push_back(buf);

	return tokens;
}

/**
 * \brief Constructor
 */
Parser::Parser()
	: clock_(0), clock_counter_(0), is_clock_calculated_(false)
{
}

/**
 * \brief Adds the given module in the modules_ vector
 * \param module Module to be added to the modules_ vector
 */
void Parser::AddModule(Module module) {
	modules_.push_back(module);
}

void Parser::AddWave(Wave wave) {
	waveform_.waveSignal.push_back(wave);
}

/**
 * \brief Displays the report of the parsed data
 */
void Parser::ShowReport() {
	std::cout << "-########################################-" << std::endl;
	std::cout << "-###  Value Change Dump (VCD) Parser  ###-" << std::endl;
	std::cout << "-########################################-" << std::endl;
	std::cout << std::endl;

	std::cout << "----------------------------------------------------------------------------" << std::endl;
	std::cout << "VCD File Path: " << vcd_file_path_ << std::endl;
	std::cout << "Simulation Date: " << simulation_date_ << std::endl;
	std::cout << "Version: " << version_ << std::endl;
	std::cout << "Clock frequency: " << clock_ << " Hz" << std::endl;
	std::cout << "----------------------------------------------------------------------------" << std::endl;
	std::cout << std::endl;

	for (auto &module : modules_) {
		std::cout << "Module: " << module.Name << std::endl;
		std::cout << std::setw(30)  << std::left << "Name" << std::right << "Symbol" << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;
		for (auto &signal : module.Signals) {	
			std::cout << std::setw(30) << std::left << signal.Name << std::right << signal.Symbol << std::endl;
		}

		std::cout << "----------------------------------------------------------------------------" << std::endl;
		std::cout << std::endl;
	}

	//Prints out the entire waveform for all waves
	waveform_.show(0, waveform_.totalTime);

	std::cout << "--------End report--------" << std::endl;
}

double Parser::GetTimeScale() const {
	auto scale = 10e-9;
	if (time_scale_.compare("1ns") == 0)
		scale = 10e-9;
	else if (time_scale_.compare("1us") == 0)
		scale = 10e-6;
	else if (time_scale_.compare("1ms") == 0)
		scale = 10e-3;

	return scale;
}

/**
 * \brief Calculates the clock frequency 
 */
void Parser::CalculeClockFrequency() {
	if ((line_.size() > 0) && (line_.at(0) == '#') && (clock_counter_ < 2)) {	
		std::string::size_type sz;
		auto numberStr = line_.substr(1, std::string::npos);

		if (clock_counter_ == 1) {
			auto period = std::stoi(numberStr, &sz);
			clock_ = 1.0 / (period*GetTimeScale());
			is_clock_calculated_ = true;
		}
		clock_counter_++;
	}
}

void Parser::RemoveTabFromString(std::string& str) const {
	str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
}

/**
 * \brief Parses the Date when the simulation was executed
 */
void Parser::ParseDate() {
	if (line_.find("$date") != std::string::npos) {
		while (getline(vcd_file_, line_)) {
			if (line_.find(":") != std::string::npos) {
				if (line_.find('\t') != std::string::npos) {
					simulation_date_ = line_;
					RemoveTabFromString(simulation_date_);
				}
				else {
					simulation_date_ = line_;
				}
				break;
			} else {
				simulation_date_ = line_;
				RemoveTabFromString(simulation_date_);
				break;
			}
		}
	}
}


/**
 * \brief Parses the Simulation tool version
 */
void Parser::ParseVersion() {
	if (line_.find("$version") != std::string::npos) {
		while (getline(vcd_file_, line_)) {
			if (line_.find("Version") != std::string::npos) {
				if (line_.find('\t') != std::string::npos) {
					version_ = line_;
					RemoveTabFromString(version_);
				}
				else {
					version_ = line_;
				}
				break;
			} else {
				version_ = line_;
				RemoveTabFromString(version_);
				break;
			}
		}
	}
}


/**
 * \brief Parses the time scale used for the simulation
 */
void Parser::ParseTimeScale() {
	if(line_.find("$timescale") != std::string::npos) {
		while (getline(vcd_file_, line_)) {
			if ((line_.find("1") != std::string::npos) || (line_.find("s") != std::string::npos)) {
				time_scale_ = line_;
				break;
			}
		}
	}
}

/**
 * \brief Parses the provide .vcd file
 * \param vcdFilePath Full path of .vcd file
 * \return true whether the parsing was succesful else false
 */
bool Parser::Parse(std::string vcdFilePath) {
	auto result = false;
	bool error = false;
	vcd_file_.open(vcdFilePath);

	waveform_.smallestInterval = INT_MAX;
	int currentClock = 0; //Initializes current clock;
	int maxSignalDigits = 1; //Used for displaying waveform neatly. Initialized to 1 as there would be signals with at least 1 digit
	if (vcd_file_.is_open()) {
		vcd_file_path_ = vcdFilePath;

		waveform_.longestSignalName = 0;
		while (getline(vcd_file_, line_)) {
			ParseDate();

			ParseVersion();

			ParseTimeScale();

			if (line_.find("$scope") != std::string::npos) {
				auto splittedLine = SplitBySpace(line_);
				Module module;
				module.Name = splittedLine[2];
				//Insert a new string for name
				while (getline(vcd_file_, line_)) {
					if (line_.find("$var") != std::string::npos) {
						Signal signal;
						signal.SwitchingActivityCounter = -1;
						auto splittedLineSignal = SplitBySpace(line_);
						signal.Symbol = splittedLineSignal[3];
						Wave wave;
						wave.symbol = signal.Symbol;
						wave.binary = true; //Used for parsing, if not only 1, 0 or x's, can print out full number instead of digital wave
						if (splittedLineSignal.size() == 6) { //Insert new signal name
							signal.Name = splittedLineSignal[4];
							wave.name = signal.Name;
						}
						else {
							signal.Name = splittedLineSignal[4] + ' ' + splittedLineSignal[5];
							wave.name = signal.Name;
						}
						if(wave.name.length() > waveform_.longestSignalName) {
							waveform_.longestSignalName = wave.name.length();
						}
						module.Signals.push_back(signal);
						AddWave(wave);
					}
					else {
						break;
					}
				}
				AddModule(module);
			}
			else if (line_.find("$dumpvars") != std::string::npos) {
				//char switchingActivitySignals[SwitchingActivitySignalsSize] = { '0', '1', 'x' };

				//auto foundSwitchingActivity = false;

				std::vector<std::string> currentClockSignals; //Error checking for setting a signal multiple times in a clock cycle
				while (getline(vcd_file_, line_)) { //Goes through rest of file once variables are initialized
					if (!IsClockCalculated())
						CalculeClockFrequency();
					if(line_[0] == '#') {
						currentClockSignals.clear();

						int clockCurrent = stoi(line_.substr(1, line_.size())); //The VCD file's current line that also is the clock

						if(clockCurrent < currentClock) {
							std::cout << "Error: VCD clock is not in order. Cannot parse." << std::endl;
							error = true;
						}

						if(clockCurrent - currentClock < waveform_.smallestInterval ) {
							waveform_.smallestInterval = clockCurrent - currentClock;
						}
						currentClock = stoi(line_.substr(1, line_.size()));
					}
					else if(line_[0] == '$') {
						continue;
					}
					// Check if line_ is 3 char long (value and symbol and carriage return)
					else if ((line_.length() <= 3) && ((line_[0] == '0') || (line_[0] == '1') || (line_[0] == 'x'))) {
						auto lineSubStr = line_.substr(1, line_.size());
						std::cout << "Line substr binary: " << lineSubStr << std::endl;
						if (!lineSubStr.empty() && lineSubStr[lineSubStr.size() - 1] == '\r') //Removes carriage return at end of line
							lineSubStr.erase(lineSubStr.size() - 1);
						
						if(std::find(currentClockSignals.begin(), currentClockSignals.end(), lineSubStr) != currentClockSignals.end()) {
							std::cout << "Error: Cannot set signal multiple times in same clock: " << lineSubStr << " Clock: " << currentClock << std::endl;
							error = true;
						} else {
							currentClockSignals.push_back(lineSubStr);
						}	

						for (std::vector<Wave>::iterator wave = waveform_.waveSignal.begin(); wave != waveform_.waveSignal.end(); ++wave) {
							if (lineSubStr.compare(wave->symbol)==0) {
								//Store signal in Pair here
								std::pair<int, std::string> timeSignal;
								timeSignal.first = currentClock;
								timeSignal.second = line_[0];
								(wave->timeSignal).push_back(timeSignal);
							}	
						}
					} 
					else { //Other signals that have non binary values
						auto lineSubStr = line_.substr(line_.size()-2, line_.size());
						std::cout << "Line substr nonbinary: " << lineSubStr << std::endl;
						auto value = line_.substr(0, line_.size() - 2);
						if (!lineSubStr.empty() && lineSubStr[lineSubStr.size() - 1] == '\r') //Removes carriage return at end of line
							lineSubStr.erase(lineSubStr.size() - 1);
						
						if(std::find(currentClockSignals.begin(), currentClockSignals.end(), lineSubStr) != currentClockSignals.end()) {
							std::cout << "Error: Cannot set signal multiple times in same clock: " << lineSubStr << " Clock: " << currentClock << std::endl;
							error = true;
						} else {
							currentClockSignals.push_back(lineSubStr);
						}	

						for (std::vector<Wave>::iterator wave = waveform_.waveSignal.begin(); wave != waveform_.waveSignal.end(); ++wave) {
							if (lineSubStr.compare(wave->symbol)==0) {
								//Store signal in Pair here
								wave->binary = false; //Because the value found is not 0, 1 or x so entire wave print numbers

								std::pair<int, std::string> timeSignal;
								timeSignal.first = currentClock;
								timeSignal.second = value; //Stores everything but last character and carriage return
								(wave->timeSignal).push_back(timeSignal);
							}	
						}
					}
				}
			}
		}
		if(error) {
			return false;
		}

		waveform_.totalTime = currentClock;
		waveform_.longestSignalDigits = maxSignalDigits;
		vcd_file_.close();
		CalculateSignalCounter();
		result = true;
	}
	return result;
}

void Parser::CalculateSignalCounter() {
	for (auto &module : modules_) {
		unsigned signalCounter = 0;
		auto previousSignal = module.Signals[0];
		for (auto it = module.Signals.begin(); it != module.Signals.end(); ++it) {	
			if (signalCounter == 0) {
				signalCounter++;
			}
			else {
				if (it->Name.find(" ") != std::string::npos) {
					auto signalName = SplitBySpace(it->Name)[0];
					if (previousSignal.Name.find(" ") != std::string::npos) {
						auto previossignalName = SplitBySpace(previousSignal.Name)[0];
						if (previossignalName.compare(signalName) != 0) {
							signalCounter++;
						}
					}
					else {
						signalCounter++;
					}
				}					
				else {
					signalCounter++;
				}
			}
			previousSignal = *it;
		}
		module.SignalCounter = signalCounter;
	}
}
