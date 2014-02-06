#include <fstream>
#include <sstream>

#ifndef UTILITIES_H
#define UTILITIES_H

class debugOutput{
    std::ofstream myDebugfile;
    
public:
    debugOutput(int rank, int threadId);
    ~debugOutput();
    
    void writeLog(std::string msg);
};


//
// Others generic stuff
std::string numToStr(int num);

#endif