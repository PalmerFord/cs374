/* parallelLoopChunks.c
 *
 *  ... demonstrates the parallel for loop pattern in MPI
 *  in which processes perform contiguous 'chunks' of the loop's iterations.
 *  If the number of iterations is evenly divisible by the num of processes,
 *  then each 'chunk' is the same size;
 *  otherwise, the size of each chunk differs by at most 1.
 *
 *  See: parallelLoopChunks.h for function getChunkStartStopValues(),
 *       which computes the key loop-iteration values for an MPI process.
 *
 * Joel Adams, Calvin University, Fall 2023.
 *
 * Usage: mpirun -np N ./parallelLoopChunks
 *
 * Exercise:
 * - Compile and run, varying N: 1, 2, 3, 4, 5, 6, 7, 8, 9
 * - Change REPS to 16, save, recompile, rerun, varying N again.
 * - Explain how this pattern divides the iterations of the loop
 *    among the processes.
 */

#include "parallelLoopChunks.h"               // MPI, getChunkStartStopValues()

int main(int argc, char** argv) {
    const unsigned REPS = 8;
    int id = -1, numProcesses = -1;
    unsigned start = -1, stop = -1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    getChunkStartStopValues(id, numProcesses, REPS, &start, &stop);

    for (unsigned i = start; i < stop; ++i) { // iterate through our chunk
        printf("Process %d is performing iteration %u\n", id, i);
    }
 
    MPI_Finalize();
    return 0;
}

