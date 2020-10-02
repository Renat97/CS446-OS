//
// Created by Renat Norderhaug on 2/5/19.
//

#include <iostream>
#include <string>

#include "MetaDataCode.h"

// parmatererized constructor
MetaDataCode::MetaDataCode(char codeInput, std::string descriptorInput, int cyclesInput, std::string dataInput) {
    this->code = codeInput;
    this->descriptor = descriptorInput;
    this->cycles = cyclesInput;
    this->data = dataInput;
}

// sets the processing time after construction
void MetaDataCode::setProcessingTime(int processingTimeInput) {
    this->processingTime = processingTimeInput;
}

// getters for metadata
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

int MetaDataCode::getProcessingTime() {
    return this->processingTime;
}

