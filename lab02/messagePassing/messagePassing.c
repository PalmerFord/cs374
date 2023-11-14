/* messagePassing.c
 * ... illustrates the use of the MPI_Send() and MPI_Recv() commands...
 *
 * Joel Adams, Calvin University, CS 374, November 2009.
 *
 * Usage: mpirun -np N ./messagePassing
 *
 * Precondition:  N is even && N > 1
 */

#include <stdio.h>   // printf()
#include <stdlib.h>  // exit()
#include <mpi.h>     // MPI
#include <math.h>    // sqrt()

/* Utility function to identify odd numbers
 * @param: number, an int.
 * @return: true, iff number's value is odd.
 */
int odd(int number) { return number % 2; }

int main(int argc, char** argv) {
    const int MASTER = 0;
    int id = -1, numProcesses = -1; 
    float sendValue = -1, receivedValue = -1;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    // Check precondition before proceeding
    if (odd(numProcesses) || numProcesses <= 1) {
        if (id == MASTER) {                           // Master-only
            printf("\n*** Run this program using -np N where N is positive and even.\n\n");
        }
        MPI_Finalize();
        exit(1);
    }

    sendValue = sqrt(id);
    if ( odd(id) ) {  // odd processors send, then receive 
        MPI_Send(&sendValue, 1, MPI_FLOAT, id-1, 1, MPI_COMM_WORLD);
        MPI_Recv(&receivedValue, 1, MPI_FLOAT, id-1, 2, 
                       MPI_COMM_WORLD, &status);
    } else {          // even processors receive, then send 
        MPI_Recv(&receivedValue, 1, MPI_FLOAT, id+1, 1, 
                       MPI_COMM_WORLD, &status);
        MPI_Send(&sendValue, 1, MPI_FLOAT, id+1, 2, MPI_COMM_WORLD);
    }

    printf("Process %d of %d computed %f and received %f\n",
                id, numProcesses, sendValue, receivedValue);

    MPI_Finalize();
    return 0;
}

