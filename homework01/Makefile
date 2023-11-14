PROG1   = circuitSatisfiabilitySlices
PROG2   = circuitSatisfiabilityChunks
PROGS   = $(PROG1) $(PROG2)
CC      = mpicc
CFLAGS  = -Wall
LFLAGS1 = -o $(PROG1) -lm
LFLAGS2 = -o $(PROG2) -lm

$(PROGS): $(PROG1) $(PROG2)

$(PROG1): $(PROG1).c
	module load openmpi-2.0/gcc; \
	$(CC) $(CFLAGS) $(PROG1).c $(LFLAGS1)

$(PROG2): $(PROG2).c
	module load openmpi-2.0/gcc; \
	$(CC) $(CFLAGS) $(PROG2).c $(LFLAGS2)

clean:
	rm -f $(PROG1) $(PROG2) a.out *~ *# *.o *.out slurm*

