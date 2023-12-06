/* squareAndSum.c computes the sum of the squares
 *  of the values in an input file,
 *  whose name is given on the command-line.
 *
 * The program is written using typedef to declare
 *  Item as a generic type, currently double.
 *
 * Joel Adams, Fall 2023
 * for CS 374 (HPC) at Calvin University.
 * 
 * Palmer Ford
 * Calvin University
 * November 27, 2023
 * for CS374, project 4
 */

#include <stdio.h>      /* I/O */
#include <stdlib.h>     /* calloc(), exit(), etc. */
#include <mpi.h>

typedef double Item;

void readArray(char * fileName, Item ** a, int * n);
double arraySquareAndSum(Item* a, int numValues);

int main(int argc, char * argv[])
{
  MPI_Init(&argc, &argv);
  double start_time = MPI_Wtime();
  int  howMany;
  Item sum;
  Item* a;

  if (argc != 2) {
    fprintf(stderr, "\n*** Usage: squareAndSum <inputFile>\n\n");
    exit(1);
  }
  
  readArray(argv[1], &a, &howMany);
  double afterArrTime = MPI_Wtime() - start_time;
  double sum_start = MPI_Wtime();
  sum = arraySquareAndSum(a, howMany);
  double sumTime = MPI_Wtime() - sum_start;
  printf("The sum of the squares of the values in the file '%s' is %g\nTimes: %f, %f, %f\n", argv[1], sum, afterArrTime, sumTime, afterArrTime + sumTime);

  free(a);

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