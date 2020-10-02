//
// Created by Renat Norderhaug on 2/15/19.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

#include "Config.h"

// the default constructor for the Config file.
Config::Config() {}

// It reads the entire file into a string, then parses the string with tokens. it is used in the main file when reading it in.
void Config::readConfigFile(std::string configPath) {
    // reading in the input file
    std::ifstream configFile;
    configFile.open(configPath);

    // error for not able to open the file, checks to make sure the extension is .conf
    if(!configFile || configPath.substr(configPath.find_last_of(".") + 1) != "conf"  ) {
        std::cerr << "bad file name, unopen to open file.\n";
        exit(1);
    }

    // vector for each individual token
    std::vector<std::string> tokens;
    std::string s;

    // reads a string of text until whitespace and then pushes until cant read in the string
    while(configFile >> s) {
        tokens.push_back(s);
    }

    // iterates vector and checks the values
    for(auto it = tokens.begin(); it != tokens.end(); it++) {
        if(*it == "Log:") {
            this->logTo = *(it+3);
        } else if(*it == "File") {
            s = *(it+2);
            if(s.find(".mdf") != std::string::npos) {
                this->filePath = s;
            }
            // string to float with the version
        } else if(*it == "Version/Phase:") {
            this->version = std::stof(*(it+1));
        } else if(*it == "Log") {
            std::string temp = *(it+3);
            if(temp.find(".lgf") != std::string::npos) {
                this->logFilePath = temp;
            }
        } else if(*it == "Monitor") {
            this->monitorDisplayTime = std::stoi(*(it+4));
        } else if(*it == "Processor") {
            this->processorCycleTime = std::stoi(*(it+4));
        } else if(*it == "Scanner") {
            this->scannerCycleTime = std::stoi(*(it+4));
        } else if(*it == "Hard") {
            this->harddriveCycleTime = std::stoi(*(it+5));
        } else if(*it == "Keyboard") {
            this->keyboardCycleTime = std::stoi(*(it+4));
        } else if(*it == "Memory") {
            this->memoryCycleTime = std::stoi(*(it+4));
        } else if(*it == "Projector") {
            this->projectorCycleTime = std::stoi(*(it+4));
        }
    }
    configFile.close();

    if(!this->validate()  ) {
        std::cerr << "Missing Data!\n" << std::endl;
        exit(1);
    }
}

// checks to see if the config file given as a parameter is valid, such as being greater than 0 ms.
bool Config::validate() {
    bool monitor = monitorDisplayTime > 0;
    bool processor = this->processorCycleTime > 0;
    bool scanner = this->scannerCycleTime > 0;
    bool harddrive = this->harddriveCycleTime > 0;
    bool keyboard = this->keyboardCycleTime > 0;
    bool memory = this->memoryCycleTime > 0;
    bool projector = this->projectorCycleTime > 0;

    bool ver = this->version > 0;

    return monitor && processor && scanner && harddrive && keyboard && memory && projector && ver;
}
 // gets the file path we read from the file
std::string Config::getFilePath() {
    return this->filePath;
}
// getters returning the Time
int Config::getMDT() {
    return this->monitorDisplayTime;
}

int Config::getPCT() {
    return this->processorCycleTime;
}

int Config::getSCT() {
    return this->scannerCycleTime;
}

int Config::getHCT() {
    return this->harddriveCycleTime;
}

int Config::getKCT() {
    return this->keyboardCycleTime;
}

int Config::getMemCT() {
    return this->memoryCycleTime;
}

int Config::getProCT() {
    return this->projectorCycleTime;
}

std::string Config::getLogTo() {
    return this->logTo;
}

std::string Config::getLogFilePath() {
    return this->logFilePath;
}
