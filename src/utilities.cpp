#include "utilities.h"

#include <sys/stat.h>
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif


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

std::string numToStr(int num){
    std::ostringstream ss;
    ss << num;
    return ss.str();
}

void createPpmFloat(float array[], int dimx, int dimy, std::string filename){
    int i, j;
    std::cout << "createPpm2  dims: " << dimx << ", " << dimy << " -  " << filename.c_str() << std::endl;
    FILE *fp = fopen(filename.c_str(), "wb"); // b - binary mode 
    (void) fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);
    for (j = 0; j < dimy; ++j){
        for (i = 0; i < dimx; ++i){
        	//std::cout << array[j*(dimx*4) + i*4 + 3] << "   ";
            static unsigned char color[3];
            float alpha = array[j*(dimx*4) + i*4 + 3];
            color[0] = array[j*(dimx*4) + i*4 + 0] * alpha * 255;  // red
            color[1] = array[j*(dimx*4) + i*4 + 1] * alpha * 255;  // green
            color[2] = array[j*(dimx*4) + i*4 + 2] * alpha * 255;  // blue 
            (void) fwrite(color, 1, 3, fp);
        }
    }
    (void) fclose(fp);
    std::cout << "End createPpm: " << std::endl;
}

// map from (a1, a2) -> (b1, b2)
float map(float value, float a1, float a2, float b1, float b2){
	return b1 + ((b2 - b1) / (a2 - a1)) * (value - a1);
}

std::string getCurrentPath(){

	char cCurrentPath[FILENAME_MAX];

    if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))){
		std::cout << "ERROR!" << std :: endl;
		return "";
    } else{
		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
		return cCurrentPath;
	}

}