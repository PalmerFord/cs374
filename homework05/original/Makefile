PROG    = ./mandel
SRC     = $(PROG).cpp
OBJ     = $(PROG).o

CC      = g++
CFLAGS  = -c -Wall -ansi -pedantic -std=c++11 \
	  -I/usr/include/TSGL \
	  -I/usr/include/freetype2 \
	  -I/usr/include/freetype2/freetype \
	  -fopenmp
LFLAGS  = -o $(PROG) -lm \
	  -ltsgl -lfreetype -lGL -lGLEW -lglfw -lXrandr \
	  -fopenmp

$(PROG): $(OBJ)
	$(CC) $(OBJ) $(LFLAGS)
	
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC) 

clean:
	rm -f $(PROG) $(OBJ) *~ *#

