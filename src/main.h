//********************************************************
// 
//
//********************************************************
#include <stdlib.h>

#include <mpi.h>
#include <pthread.h>

#include <iostream>

#include "utilities.h"

// threads
int numThreads = 8;     // number of threads/cores on a machine

// mpi
int mpiWorldSize;       // size ofn mpi world   
int my_id;              // mpi id