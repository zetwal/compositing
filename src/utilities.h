// ************************************************************************* //
//                                utilities.h                                //
// ************************************************************************* //

#include <fstream>
#include <sstream>

#ifndef UTILITIES_H
#define UTILITIES_H

class debugOutput{
    std::ofstream myDebugfile;
    
public:
    debugOutput(int rank, int threadId);
    ~debugOutput();
    
    void closeFile();
    void writeLog(std::string msg);
};



//
// Others generic stuff
std::string numToStr(int num){
    std::ostringstream ss;
    ss << num;
    return ss.str();
}

#endif