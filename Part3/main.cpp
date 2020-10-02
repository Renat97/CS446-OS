/// main.cpp made by Renat Norderhaug
/// CS 446 project 3
/// 4/7/2019


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <pthread.h>
#include <chrono>
#include <sstream>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#include "Config.cpp"
#include "MetaDataCode.cpp"


struct processState {
	int state;
};

// functions to find processing time
void calculateProcessingTime(Config*, MetaDataCode&, int&, int&);
void readMetaDataFile(std::string, std::vector<MetaDataCode>&);
void outputToLogFile(Config, std::vector<MetaDataCode>);
void output(Config, std::vector<MetaDataCode>, std::ostream& );
void* timer(void*);
double processThread(processState);
void runProcess(int, MetaDataCode&);
void initSem(Config);

/// logger class functions implemented here
void directOutput(Config, std::string, int a);
void outputToStream(std::ostream&, std::string);

void processOperation(int, MetaDataCode&, sem_t&, pthread_mutex_t&);
std::string generateMemoryLocation(int);


 ///enum { START = 0, READY = 1, RUNNING = 2, WAIT = 3, EXIT = 4 }
const  int START = 0;
const  int READY = 1;
const  int RUNNING = 2;
const  int WAIT = 3;
const  int EXIT = 4;

/// Resource class's data, semaphores for signaling and telling thread to wait, lock is for
/// acquiring resource
sem_t projectorSemaphore;
sem_t harddriveSemaphore;
sem_t scannerSemaphore;
sem_t monitorSemaphore;
sem_t keyboardSemaphore;

pthread_mutex_t projectorLock;
pthread_mutex_t harddriveLock;
pthread_mutex_t scannerLock;
pthread_mutex_t monitorLock;
pthread_mutex_t keyboardLock;


 int harddriveOutCount = 0;
 int harddriveInCount = 0;
 int projectorCount = 0;
 int memoryBlocksAllocated = 0;

static bool didFirstLog = false;

/// state of the process
processState ps;

/// timer
const auto START_TIME = std::chrono::system_clock::now();
static int TIMER = 0;

/// Simulation starts here

int main(int argc, char *argv[]) {
	/// Instantiate Config and MetaData
	ps.state = EXIT;
	Config cf;
	std::string configFilePath = argv[1];
	std::vector<MetaDataCode> MetaDataVector;

	int systemStatus = 0;
	int applicationStatus = 0;

	cf.readConfigFile(configFilePath);

	readMetaDataFile(cf.getFilePath(), MetaDataVector);
	initSem(cf);

	for(auto& mdc : MetaDataVector) {
		calculateProcessingTime(&cf, mdc, systemStatus, applicationStatus);
	}

	return 0;
}

/// timer class implementation
void* timer(void *emp) {

	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;

	double limit = 0.1;

	std::chrono::duration<double> elapsedTime;

	start = std::chrono::system_clock::now();

	while(elapsedTime.count() < limit) {
		end = std::chrono::system_clock::now();
		elapsedTime = end - start;
	}

	pthread_exit(NULL);
	return NULL;
}
/// global function, doesn' t parse data, finds the end time for thread.
double processThread(int timeLimit) {

	int time = timeLimit;
	pthread_t timerThread;

	auto timeEnd = std::chrono::system_clock::now() + std::chrono::milliseconds(timeLimit);

	while(std::chrono::system_clock::now() < timeEnd) {
		pthread_create(&timerThread, NULL, timer, NULL);
		pthread_join(timerThread, NULL);
	}

	auto currentTime = std::chrono::system_clock::now();

	return std::chrono::duration<double>(currentTime-START_TIME).count();
}

std::string generateMemoryLocation(int memory) {
	std::string input;

	std::stringstream sstr;
	sstr << std::hex << std::setw(8) << std::setfill('0') << memory;

	return sstr.str();
}


/// Gets code and validates descriptor and calculates process time

void calculateProcessingTime(Config* cf, MetaDataCode& mdc, int& systemStatus, int& applicationStatus) {
	int timeLimit;
	int p = 1;

	if(mdc.getCode() == 'S') {
		if(mdc.getDescriptor() == "begin" && ps.state == EXIT) {
			ps.state = START;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Simulator program starting", p);
		} else if(mdc.getDescriptor() == "finish" && ps.state == READY) {
			ps.state = EXIT;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setProcessingTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Simulator program ending" ,p);
		} else {

			++p;
			void calculateProcessingTime(Config* cf, MetaDataCode& mdc, int& systemStatus, int& applicationStatus);

		}
	} else if(mdc.getCode() == 'A') {
		if(mdc.getDescriptor() == "begin" && ps.state == START) {
			ps.state = READY;
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "OS: starting process ", p);
		} else if(mdc.getDescriptor() == "finish" && ps.state == READY) {
			auto currentTime = std::chrono::system_clock::now();
			mdc.setProcessingTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "OS: removing process ", p);
		} else {

			++p;
			void calculateProcessingTime(Config* cf, MetaDataCode& mdc, int& systemStatus, int& applicationStatus);

		}
	} else if(mdc.getCode() == 'P' && mdc.getDescriptor() == "run") {
		ps.state = RUNNING;
		timeLimit = mdc.getCycles() + cf->getPCT();
		/// set start time for right now
		auto currentTime = std::chrono::system_clock::now();
		mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
		///now output start log
		directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + " start processing action for process", p);
		///output end time
		auto endTime = processThread(timeLimit);
		mdc.setProcessingTime(endTime);
		directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end processing action", p);
		ps.state = READY;
	} else if(mdc.getCode() == 'I') {
		if(mdc.getDescriptor() == "hard drive") {
			int currentCount;
			if(harddriveInCount == cf->getHarddriveResources()) {
				harddriveInCount = 0;
			}
			currentCount = harddriveInCount;
			timeLimit = mdc.getCycles() + cf->getHCT();
			/// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			ps.state = RUNNING;
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			///now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + " start hard drive input on HDD for Process " + std::to_string(currentCount), p);
			processOperation(timeLimit, mdc, harddriveSemaphore, harddriveLock);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end hard drive input on HDD " + std::to_string(currentCount), p);
			ps.state = READY;
		} else if(mdc.getDescriptor() == "keyboard") {
			timeLimit = mdc.getCycles() + cf->getKCT();
			/// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			ps.state = RUNNING;
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			/// output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + " start keyboard output for Process", p);
			processOperation(timeLimit, mdc, keyboardSemaphore, keyboardLock);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end keyboard output", p);
			ps.state = READY;
		} else if(mdc.getDescriptor() == "scanner") {
			timeLimit = mdc.getCycles() + cf->getSCT();
			/// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			ps.state = RUNNING;
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			///now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start scanner output", p);
			processOperation(timeLimit, mdc, scannerSemaphore, scannerLock);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end scanner output,", p);
			ps.state = READY;
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	} else if(mdc.getCode() == 'O') {
		if(mdc.getDescriptor() == "hard drive") {
			/// temporary until concurrency is needed
			int currentCount;
			if(harddriveOutCount == cf->getHarddriveResources()) {
				harddriveOutCount = 0;
			}
			currentCount = harddriveOutCount++;

			timeLimit = mdc.getCycles() + cf->getHCT();
			/// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			ps.state = RUNNING;
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start hard drive output on HDD " + std::to_string(currentCount), p);
			processOperation(timeLimit, mdc, harddriveSemaphore, harddriveLock);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end hard drive output on HDD " + std::to_string(currentCount), p);
			ps.state = READY;
		} else if(mdc.getDescriptor() == "monitor") {
			/// temporary until concurrency is needed
			timeLimit = mdc.getCycles() + cf->getMDT();
			///set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			ps.state = RUNNING;
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			///now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start monitor output", p);
			processOperation(timeLimit, mdc, monitorSemaphore, monitorLock);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end monitor output", p);
			ps.state = READY;
		} else if(mdc.getDescriptor() == "projector") {
			int currentCount;
			if(projectorCount == cf->getProjectorResources()) {
				projectorCount = 0;
			}
			currentCount = projectorCount++;
			ps.state = RUNNING;
			// temporary until concurrency is needed
			timeLimit = mdc.getCycles() + cf->getProCT();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			ps.state = RUNNING;
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start projector output on PROJ " + std::to_string(currentCount), p);
			processOperation(timeLimit, mdc, projectorSemaphore, projectorLock);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end projector output on PROJ " + std::to_string(currentCount), p);
			ps.state = READY;
		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	} else if(mdc.getCode() == 'M') {
		if(mdc.getDescriptor() == "block") {
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getMemCT();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: start memory blocking", p);
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: end memory blocking", p);
			ps.state = READY;
		} else if(mdc.getDescriptor() == "allocate") {
			ps.state = RUNNING;
			timeLimit = mdc.getCycles() + cf->getMemCT();
			int memory = memoryBlocksAllocated * cf->getMemory();
			// set start time for right now
			auto currentTime = std::chrono::system_clock::now();
			mdc.setStartTime(std::chrono::duration<double>(currentTime-START_TIME).count());
			//now output start log
			directOutput(*cf, std::to_string(mdc.getStartTime()) + " - " + "Process 1: allocating memory", p);
			//output end time
			auto endTime = processThread(timeLimit);
			mdc.setProcessingTime(endTime);
			directOutput(*cf, std::to_string(mdc.getProcessingTime()) + " - " + "Process 1: memory allocated at 0x" + generateMemoryLocation(memory), p);
			memoryBlocksAllocated++;
			ps.state = READY;

		} else {
			std::cerr << "Invalid descriptor!" << std::endl;
			exit(1);
		}
	}
}
// for each thread
void processOperation(int timeLimit, MetaDataCode &mdc, sem_t &semaphore, pthread_mutex_t &mutex) {
			///  we need to wait for a semaphore to be available
			sem_wait(&semaphore);
			///  lock critical section with mutex, so process only works there
			pthread_mutex_lock(&mutex);
			// critical section
			// this is can be flexible as a reference to a function can be called
			auto endTime = processThread(timeLimit);
			// unlock the critical section to free it up
			pthread_mutex_unlock(&mutex);
			//output end time
			mdc.setProcessingTime(endTime);
			// signal that resource is released
			sem_post(&semaphore);
}

// takes in ofstream obj and outputs it
void outputToStream(std::ostream& out, std::string logOutput) {
	out << logOutput << std::endl;
}

/// sends the outputs of the log to a stream
void directOutput(Config cf, std::string logOutput, int p) {
	std::ofstream logFile;

	/// these flags are set depending on where the config specifies where to log to
	bool monitorFlag = false;
	bool logFileFlag = false;

	if(cf.getLogTo() == "Both") {
		monitorFlag = true;
		logFileFlag = true;
	} else if(cf.getLogTo() == "monitor") {
		monitorFlag = true;
	} else if(cf.getLogTo() == "File") {
		logFileFlag = true;
	} else {
		std::cerr << "Cannot log to " << cf.getLogTo() << "!\n";
		exit(1);
	}

	if(monitorFlag) {
		outputToStream(std::cout, logOutput);
	}

	if(logFileFlag) {
		logFile.open(cf.getLogFilePath(), didFirstLog ? std::ofstream::app : std::ofstream::trunc);

		if(!didFirstLog) {
			didFirstLog = true;
		}

		outputToStream(logFile, logOutput);
		logFile.close();
	}
}

/// Gets the filepath from the config and then reads the MetaData file into the MetaDataVector

void readMetaDataFile(std::string filePath, std::vector<MetaDataCode>& MetaDataVector) {
	std::ifstream metaDataFile;
	metaDataFile.open(filePath);

	std::vector<std::string> tokens;
	std::string s;
	std::string temp;

	char codeInput;
	std::string descriptorInput;
	int cyclesInput;
	bool readOverFlag = false;

	while(metaDataFile >> s) {
		if(s.find("hard") != std::string::npos) {
			metaDataFile >> temp;
			s.append(" ");
			s.append(temp);
		}

		tokens.push_back(s);
	}

	metaDataFile.close();

	// iterates over vector
	for(auto it = tokens.begin(); it != tokens.end(); it++) {
		if(*it == "Start") {
			std::advance(it, 4);
		} else if(*it == "End") {
			break;
		} else if(readOverFlag) {
			// Gets thrown if another process is read after the period '.'
			std::cerr << "Cannot find end!" << std::endl;
			exit(1);
		}

		s = *it;

		codeInput = s[0];
		s.erase(0, 2);

		descriptorInput = s.substr(0, s.find('}'));
		s.erase(0, s.find('}')+1);

		// Gets the cycle number
		temp = s.substr(0, s.find(';'));

		/// this finds out if the process ends in a period

		if(temp.find('.') != std::string::npos) {
			readOverFlag = true;
			s.erase(s.find('.'), 1);
		}

		cyclesInput = std::stoi(temp);

		/// erases semicolon
		s = *it;
		s.erase(std::remove(s.begin(), s.end(), ';'), s.end());

		MetaDataCode mdcTemp(codeInput, descriptorInput, cyclesInput, s);
		MetaDataVector.push_back(mdcTemp);
	}
}
// multi threading for each CPU operation.
void initSem(Config cf) {
	pthread_mutex_init(&projectorLock, NULL);
	pthread_mutex_init(&harddriveLock, NULL);
	pthread_mutex_init(&monitorLock, NULL);
	pthread_mutex_init(&scannerLock, NULL);
	pthread_mutex_init(&keyboardLock, NULL);

	/// initalized number of resources
	sem_init(&harddriveSemaphore, 0, cf.getHarddriveResources());
	sem_init(&monitorSemaphore, 0, 1);
	sem_init(&keyboardSemaphore, 0, 1);
	sem_init(&scannerSemaphore, 0, 1);
	sem_init(&projectorSemaphore, 0, cf.getProjectorResources());
}
