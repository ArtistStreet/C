CC = gcc
CFLAGS = -Wall
# LDFLAGS = -lreadline
SRC = dir.c hash.c mkdir.c mv.c system.c \
remove.c pwd.c ls.c touch.c help.c cd.c \
globals.c

OBJ = dir.o

my_program: $(SRC)
	$(CC) $(CFLAGS) -o my_program $(SRC) 
	./my_program 

run: my_program 
	./my_program

clean: 
	rm -f my_program *.o filesystem.txt