#!/bin/bash
#
# Set the number of nodes to use (max 20)
#SBATCH -N 1
#
# Set the number of processes per node (max 16)
#SBATCH --ntasks-per-node=1
#

# Load the compiler and MPI library
module load openmpi-2.0/gcc

# Run the program
mpirun ./Fire
