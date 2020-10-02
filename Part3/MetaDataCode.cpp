/// metadatacode class by Renat Norderhaug
/// cs 446
/// 4/7/19
#include <iostream>
#include <string>

#include "MetaDataCode.h"

// constructor
MetaDataCode::MetaDataCode(char codeInput, std::string descriptorInput, int cyclesInput, std::string dataInput) {
	this->code = codeInput;
	this->descriptor = descriptorInput;
	this->cycles = cyclesInput;
	this->data = dataInput;
}

/// no need for getter of processing time 
void MetaDataCode::setProcessingTime(double processingTimeInput) {
	this->processingTime = processingTimeInput;
}

void MetaDataCode::setStartTime(double startTimeInput) {
	this->startTime = startTimeInput;
}

/// getters
char MetaDataCode::getCode() {
	return this->code;
}

std::string MetaDataCode::getDescriptor() {
	return this->descriptor;
}

int MetaDataCode::getCycles() {
	return this->cycles;
}

std::string MetaDataCode::getData() {
	return this->data;
}

double MetaDataCode::getProcessingTime() {
	return this->processingTime;
}

double MetaDataCode::getStartTime() {
	return this->startTime;
}
