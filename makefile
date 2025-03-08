CC = gcc
CFLAGS = -Wall
LDFLAGS = -lreadline
SRC = dir.c
OBJ = dir.o

my_program: $(SRC)
	$(CC) $(CFLAGS) -o my_program $(SRC) $(LDFLAGS)
	./my_program 

run: my_program 
	./my_program

clean: 
	rm -f my_program *.o filesystem.txt