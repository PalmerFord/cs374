/* parSquareAndSum.c computes the sum of the squares
 *  of the values in an input file,
 *  whose name is given on the command-line.
 *
 * The program is written using typedef to declare
 *  Item as a generic type, currently double.
 *
 * Joel Adams, Fall 2023
 * for CS 374 (HPC) at Calvin University.
 */

#include <stdio.h> /* I/O */
#include <stdlib.h> /* calloc(), exit(), etc. */
#include <mpi.h>
#include <vector> 
#include "OO_MPI_IO.h" // ParallelReader

typedef double Item;

void readArray(char * fileName, Item ** a, int * n);
double vectorSquareAndSum(std::vector<double> val);
char* processCmdLineArgs(int argc, char** argv);
void check(FILE* fptr, char* fileName);
long getFileSize(FILE* fPtr);

int main(int argc, char * argv[])
{
  Item sum;
  Item totalSum;

  double startTime = 0.0;
  double readTime = 0.0;
  double totalTime = 0.0;

  double computeTime = 0.0;
  double computeStart = 0.0;

  const int MASTER = 0;
  int id = -1, numProcs = -1;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  startTime = MPI_Wtime();

  char* fileName = processCmdLineArgs(argc, argv);

  ParallelReader<double> reader(fileName, MPI_DOUBLE, id, numProcs);
  std::vector<double> vec;  
  reader.readChunk(vec);

  if (argc != 2) {
    fprintf(stderr, "\n*** Usage: squareAndSum <inputFile>\n\n");
    exit(1);
  }
  readTime = MPI_Wtime() - startTime;
  computeStart = MPI_Wtime();

  sum = vectorSquareAndSum(vec);

  computeTime = MPI_Wtime() - computeStart;
  totalTime = readTime + computeTime;

  MPI_Reduce(&sum, &totalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  reader.close();

  if(id == MASTER) {
    printf("\nThe sum of the squares of the values in the file '%s' is %g\n", argv[1], totalSum);
    printf("Read time,  Compute time,  Total time\n%f,     %f,     %f\n", readTime, computeTime, totalTime);
  }

  MPI_Finalize();
  return 0;
}

/* utility to check and process the command line argument
 * @param: argc, an int
 * @param: argv, a char**
 * Precondition: argc == 2
 *            && argv[1] is the name of the input file.
 * @return: argv[1].
 */
char* processCmdLineArgs(int argc, char** argv) {
   if (argc != 2) {
      fprintf(stderr, "\n\n*** Usage: ./seqTextOut N\n\n");
      exit(1);
   }

   return argv[1];
}

/* vectorSquareAndSum sums the squares of the values
 *  in an vector of numeric Items.
 * Receive: val, a pointer to the head of an vector of Items;
 * 
 * Return: the sum of the values in the vector.
 */
Item vectorSquareAndSum(std::vector<double> val) {
  Item result = 0.0;

  for (double i = 0; i < val.size(); i++) {
    result += (val[i] * val[i]);
  }

  return result;
}