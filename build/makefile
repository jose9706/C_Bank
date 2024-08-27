# Variables
CC = gcc
CFLAGS = -Wall -g

# Source files in the parent directory
SRCDIR = ../
SRC = $(SRCDIR)main.c $(SRCDIR)bank.c

# Object files
OBJ = main.o bank.o

# Target to build the final executable
my_program: $(OBJ)
	$(CC) $(CFLAGS) -o $(SRCDIR)my_program $(OBJ)

# Target to compile main.c into main.o
main.o: $(SRCDIR)main.c
	$(CC) $(CFLAGS) -c $(SRCDIR)main.c

# Target to compile bank.c into bank.o
bank.o: $(SRCDIR)bank.c
	$(CC) $(CFLAGS) -c $(SRCDIR)bank.c


# Clean up
clean:
	rm -f *.o $(SRCDIR)my_program