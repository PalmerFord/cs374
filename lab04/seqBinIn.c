/* seqBinIn.c 
 *  ... reads N double values from a binary file.
 *
 * @author: Joel Adams, for CS 374 at Calvin University, Fall 2023.
 *
 * Usage: ./seqBinIn <binFileName>
 */

#include <stdio.h>              // fprint(), etc.
#include <stdlib.h>             // exit(),  ...
#include <mpi.h> 

typedef double Item;

char* processCmdLineArgs(int argc, char** argv);
void check(FILE* fptr, char* fileName);
long getFileSize(FILE* fPtr);

int main(int argc, char** argv) {

   char* fileName = processCmdLineArgs(argc, argv);

   FILE * filePtr = fopen(fileName, "rb");
   check(filePtr, fileName);

   long numBytes = getFileSize(filePtr);
   long numItems = numBytes / sizeof(Item);  // integer division
   Item* arrayPtr = (Item*) malloc( numItems*sizeof(Item) );

   fread(arrayPtr, sizeof(Item), numItems, filePtr);

   fclose(filePtr);

   printf("\n%ld doubles read from file '%s'\n\n",
		   numItems, fileName);
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
