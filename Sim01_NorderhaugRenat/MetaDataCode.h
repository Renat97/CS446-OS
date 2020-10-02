//
// Created by Renat Norderhaug on 2/5/19.
//

#ifndef CS446_METADATACODE_H
#define CS446_METADATACODE_H
#endif //CS446_METADATACODE_H
// meta data code file written by Renat Norderhaug 2/16/19


#include <iostream>
#include <string>

class MetaDataCode {
public:
    // the parameterized constructor
    MetaDataCode(char codeInput, std::string descriptorInput, int cyclesInput, std::string data);

    void setProcessingTime(int processingTimeInput);

    char getCode();
    std::string getDescriptor();
    int getCycles();
    std::string getData();
    int getProcessingTime();
private:
    char code;
    std::string descriptor;
    int cycles;
    std::string data;

    int processingTime;
};
