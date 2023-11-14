#!/bin/bash
# Example with 4 nodes, 8 processes each = 32 processes
#
# Set the number of nodes to use (max 20)
#SBATCH -N 1
#
# Set the number of processes per node (max 16)
#SBATCH --ntasks-per-node=2
#

# Load the compiler and MPI library
module load openmpi-2.0/gcc

# Run the program
mpirun ./circuitSatisfiabilityChunks