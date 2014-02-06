#include "utilities.h"

debugOutput::debugOutput(int rank, int threadId){
    myDebugfile.open(("debug_" + numToStr(rank) + "_" + numToStr(threadId) + ".vlog").c_str());
}

debugOutput::~debugOutput(){
}

void debugOutput::closeFile(){
     myDebugfile.close();
}

void debugOutput::writeLog(std::string msg){
    myDebugfile << msg << std::endl;
}


std::string numToStr(int num){
    std::ostringstream ss;
    ss << num;
    return ss.str();
}