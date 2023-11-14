/* Palmer Ford
 * Passes a message around a ring of processes using the master-worker and message-passing patterns
 * Calvin University
 * November 07, 2023
 * for CS374, project 2
 * 
 * Homework02.c
 */
#include <stdio.h>
#include <mpi.h>
#include <string.h>

int masterTask(int, int);
int workerTask(int, int);

int main(int argc, char** argv) {
  int id = -1, numWorkers = -1, length = -1;
  char hostName[MPI_MAX_PROCESSOR_NAME];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &numWorkers);
  MPI_Get_processor_name (hostName, &length);

  if ( id == 0 ) {  // process 0 is the master 
    masterTask(id, numWorkers);
  } else {          // processes with ids > 0 are workers 
    workerTask(id, numWorkers);
  }

  MPI_Finalize();
  return 0;
}


/* masterTask initiates message passing in a ring for the master process.
 * Sends and receives messages and measures the time to complete the ring.
 * 
 * parameter id, the process id
 * paramater numWorkers, the number of workers
 */
int masterTask( int id, int numWorkers ){
  int sizeOfStr = 4 * numWorkers;

  MPI_Status status;

  printf("Number of Nodes: %d\n", numWorkers); // Create a message containing its rank.

  char message[sizeOfStr]; // Create a string to send to the next node.
  char idString[5];
  sprintf(idString, "%d ", id);
  strcat(message, idString); // Create a message containing the message just received, a blank space, and their rank.

  double startTime = 0.0, totalTime = 0.0; startTime = MPI_Wtime(); // Records the start time.

  MPI_Send(message, sizeOfStr, MPI_CHAR, id + 1, 1, MPI_COMM_WORLD); // Send the message to process #1 (rank 1)
  MPI_Recv(message, sizeOfStr, MPI_CHAR, numWorkers - 1, 1, MPI_COMM_WORLD, &status); // Receive a message from the final worker process (rank n-1)
  
  totalTime = MPI_Wtime() - startTime;
  printf("%s\nTime: %f secs\n", message, totalTime); // Print the message and the time taken to traverse the ring
  return 0;
}


/* workerTask forwards a message in the ring for worker processes.
 * Appends the process rank to the message and passes it to the next worker.
 *
 * parameter id, the process id
 * paramater numWorkers, the number of workers.
 */
int workerTask(int id, int numWorkers) {
  int sizeOfStr = 4 * numWorkers;
  char message[sizeOfStr]; // Create a string to send to the next node.
  MPI_Status status;

  MPI_Recv(message, sizeOfStr, MPI_CHAR, id - 1, 1, MPI_COMM_WORLD, &status); // Recieve a message from the previous worker.
  char idString[5];
  sprintf(idString, "%d ", id);

  strcat(message, idString); // Create a new message containing the message just received, a blank space, and their rank.
  MPI_Send(message, sizeOfStr, MPI_CHAR, ( id + 1 ) % numWorkers, 1, MPI_COMM_WORLD); // Send message to the next worker.

  return 0;
}