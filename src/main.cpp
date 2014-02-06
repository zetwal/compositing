//mpirun -n 2 ./threadmpi 2

#include "main.h"


void *hello(void *threadId){
    std::cout << "Hello from thread " << *((int*)&threadId) << " on proc with rank " << my_id << " of " << mpiWorldSize << std::endl;
    debugOutput debugFile(my_id,(*((int*)&threadId)));
    debugFile.writeLog("Hello from thread " + numToStr( *((int*)&threadId) ) + " on proc with rank " + numToStr(my_id) + " of " + numToStr(mpiWorldSize));
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){

    // Parsing command line arguments for number of threads
    numThreads = strtol(argv[1], NULL, 10);
        

    //
    // Initialize MPI
    //
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &mpiWorldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    
    
    //
    // Initialize threads
    //
    pthread_t *threadHandles = NULL;
    threadHandles = new pthread_t[numThreads];
    
    //
    // Creating the threads
    for (int i=0; i<numThreads; i++){
        if ( pthread_create(&threadHandles[i], NULL, hello, (void *)(intptr_t)i) )
            std::cout << "Could not create thread " << i << " on rank " << my_id << " of size " <<  mpiWorldSize << std::endl;
    }
    
    
    if (threadHandles != NULL)
        delete []threadHandles;
    threadHandles = NULL;
    
    // 
    // Terminate MPI
    // 
    MPI_Finalize();
    
    return 0;
}