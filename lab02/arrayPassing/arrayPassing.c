/* arrayPassing.c
 * ... illustrates using MPI_Send() and MPI_Recv() commands on arrays...
 *
 * Joel Adams, Calvin College, September 2013.
 *
 * To run: mpirun -np N ./arrayPassing
 *
 * Precondition: N is even && N > 1
 */

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI
#include <stdlib.h>  // malloc(), exit()
#include <string.h>  // strlen()

/* utility function to check if a number is odd or not
 * @parameter: number, an int.
 * @return: true, iff number's value is odd.
 */
int odd(int number) { return number % 2; }

int main(int argc, char** argv) {
    const int SIZE = (MPI_MAX_PROCESSOR_NAME+32) * sizeof(char);
    const int MASTER = 0;
    int id = -1, numProcesses = -1, length = -1; 
    char * sendString = NULL;
    char * receivedString = NULL;
    char hostName[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Get_processor_name (hostName, &length);

    // Check precondition before proceeding
    if (odd(numProcesses) || numProcesses <= 1) {
        if (id == MASTER) {                              // MASTER-only
           printf("\n*** Run this program using -np N where N is positive and even.\n\n");
        }
        MPI_Finalize();
        exit(1);
    }
 
    sendString = (char*) malloc( SIZE+1 );
    receivedString = (char*) malloc( SIZE+1 );
    memset(sendString, '\0', SIZE+1);          // make sure these arrays
    memset(receivedString, '\0', SIZE+1);      //  are zeroed-out

    sprintf(sendString, "\n\tProcess %d is on host '%s'\n", id, hostName);

    if ( odd(id) ) {  // odd processes send, then receive 
        MPI_Send(sendString, strlen(sendString)+1,
                       MPI_CHAR, id-1, 1, MPI_COMM_WORLD);
        MPI_Recv(receivedString, SIZE, MPI_CHAR, id-1, 2, 
                       MPI_COMM_WORLD, &status);
    } else {          // even processes receive, then send 
        MPI_Recv(receivedString, SIZE, MPI_CHAR, id+1, 1, 
                       MPI_COMM_WORLD, &status);
        MPI_Send(sendString, strlen(sendString)+1,
                       MPI_CHAR, id+1, 2, MPI_COMM_WORLD);
    }

    printf("Process %d of %d received the message:%s\n",
                id, numProcesses, receivedString);

    free(sendString); 
    free(receivedString);
    MPI_Finalize();
    return 0;
}

