PROG   = squareAndSum
CC     = mpicc
CFLAGS = -Wall -ansi -pedantic -std=c99
LFLAGS = -o $(PROG) 

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) $(PROG).c $(LFLAGS)

clean:
	rm -f $(PROG) a.out *~ *.o *#

cleanText:
	rm -f $(PROG) *.txt

