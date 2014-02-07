#!/bin/bash

#PBS -l nodes=2:ppn=1,pmem=2GB,walltime=72:00:00
#PBS -N mpiThreads
#PBS -m abe
#PBS -M pgrosset@sci.utah.edu

PROG=/home/sci/pgrosset/compositing/compositing.git/src/mpiThreads
PROGARGS="3"
#SCRATCH_DIR=/usr/sci/cluster/pgrosset/$USER/$PBS_JOBID
 
#make sure the scratch directory is created
#mkdir -p $SCRATCH_DIR

#copy datafiles from directory where I typed qsub, to scratch directory
#cp -r $PBS_O_WORKDIR/* $SCRATCH_DIR/

#change to the scratch directory
#cd $SCRATCH_DIR

echo "Job started on `hostname` at `date`"

# Execute the mpi job
#mpirun $PROG $PROGARGS
mpirun -n 2 $PROG 3

echo "Job Ended at `date`"
 
#copy data back from scratch directory to directory where I typed qsub
#cp -r $SCRATCH_DIR/* $PBS_O_WORKDIR/

exit 0
