/* squareAndSum.c computes the sum of the squares
 *  of the values in an input file,
 *  whose name is given on the command-line.
 *
 * The program is written using typedef to declare
 *  Item as a generic type, currently double.
 *
 * Joel Adams, Fall 2023
 * for CS 374 (HPC) at Calvin University.
 */

#include <stdio.h>      /* I/O */
#include <stdlib.h>     /* calloc(), exit(), etc. */
#include <mpi.h> 

typedef double Item;

void readArray(char * fileName, Item ** a, int * n);
double arraySquareAndSum(Item* a, int numValues);
char* processCmdLineArgs(int argc, char** argv);
void check(FILE* fptr, char* fileName);
long getFileSize(FILE* fPtr);

int main(int argc, char * argv[])
{
  MPI_Init(&argc, &argv);
  double startTime = 0.0, totalTime = 0.0, readTime = 0.0, computeTime = 0.0;
  startTime = MPI_Wtime();

  char* fileName = processCmdLineArgs(argc, argv);

  FILE * filePtr = fopen(fileName, "rb");
  check(filePtr, fileName);

  long numBytes = getFileSize(filePtr);
  long numItems = numBytes / sizeof(Item);  // integer division
  Item* arrayPtr = (Item*) malloc( numItems*sizeof(Item) );

  fread(arrayPtr, sizeof(Item), numItems, filePtr);
  fclose(filePtr);

  readTime = MPI_Wtime() - startTime;
  printf("Read time: %f\n", readTime);

  double sum = arraySquareAndSum(arrayPtr, numItems);

  computeTime = MPI_Wtime() - readTime; // ERROR
  printf("Compute time: %f\n", computeTime);

  printf("The sum of the squares of the values in the file '%s' is %g\n",
           argv[1], sum);

  totalTime = MPI_Wtime() - startTime;

  printf("Total time: %f\n", totalTime);
  MPI_Finalize();

  return 0;
}

/* readArray fills an array with Item values from a file.
 * Receive: fileName, a char*,
 *          a, the address of a pointer to an Item array,
 *          n, the address of an int.
 * PRE: fileName contains N, followed by N double values.
 * POST: a points to a dynamically allocated array
 *        containing the N values from fileName
 *        and n == N.
 */

void readArray(char * fileName, Item** a, int * n) {
  int count, howMany;
  Item* tempA;
  FILE * fin;

  fin = fopen(fileName, "r");
  if (fin == NULL) {
    fprintf(stderr, "\n*** Unable to open input file '%s'\n\n",
                     fileName);
    exit(1);
  }

  fscanf(fin, "%d", &howMany);
  tempA = calloc(howMany, sizeof(Item));
  if (tempA == NULL) {
    fprintf(stderr, "\n*** Unable to allocate %d-length array",
                     howMany);
    exit(1);
  }

  for (count = 0; count < howMany; count++)
   fscanf(fin, "%lf", &tempA[count]);

  fclose(fin);

  *n = howMany;
  *a = tempA;
}

/* arraySquareAndSum sums the squares of the values
 *  in an array of numeric Items.
 * Receive: a, a pointer to the head of an array of Items;
 *          numValues, the number of values in the array.
 * Return: the sum of the values in the array.
 */

Item arraySquareAndSum(Item* a, int numValues) {
  Item result = 0.0;

  for (int i = 0; i < numValues; ++i) {
    result += (a[i] * a[i]);
  }

  return result;
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

/* utility to check that opening the file succeeded.
 * @param: fPtr, a FILE*
 * @param: fileName, a char*
 * Precondition: fPtr holds the result of a call to fopen()
 *            && fileName is the name of the arg to fopen().
 * Postcondition: if fPtr == NULL, an error message has been displayed.
 */
void check(FILE* fPtr, char* fileName) {
   if (fPtr == NULL) {
      fprintf(stderr, "\n*** Unable to open file '%s'\n\n",
                                fileName);
   }
}

/* utility to calculate the size of an open file in bytes
 * @param: fPtr, a FILE*
 * Precondition: fPtr is to a file that has just been opened.
 * @return: the number of bytes in the file.
 */
long getFileSize(FILE* fPtr) {
    // seek to the end of the file.
    fseek(fPtr, 0L, SEEK_END);

    // calculate the size of the file
    long res = ftell(fPtr);

    // return the file ptr to the beginng
    fseek(fPtr, 0L, SEEK_SET);

    return res;
}

