#include "utilities.h"

debugOutput::debugOutput(int rank, int threadId){
    myDebugfile.open(("debug_" + numToStr(threadId) + "_on_" + numToStr(rank) + ".vlog").c_str());
}

debugOutput::~debugOutput(){
}

void debugOutput::closeFile(){
     myDebugfile.close();
}

void debugOutput::writeLog(std::string msg){
    myDebugfile << msg << std::endl;
}