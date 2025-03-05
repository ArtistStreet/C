CC = gcc
CFLAGS = -Wall
SRC = dir.c
OBJ = dir.o

my_program: $(SRC)
	$(CC) $(CFLAGS) -o my_program $(SRC)
	./my_program 

run: my_program
	./my_program

clean: 
	rm -f my_program *.o