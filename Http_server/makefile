# Compiler
CC = gcc
CFLAGS = -Wall -g
MONGOC_FLAGS = $(shell pkg-config --cflags --libs libmongoc-1.0 libbson-1.0 -lssl -lcrypto)

# Files
SRC = main.c ssl.c user.c
OBJ = $(SRC:.c=.o)
TARGET = main

# Default arguments
ARGS = Web/index.html Web/style.css Web/script.js

# Build executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(MONGOC_FLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(MONGOC_FLAGS) -c $< -o $@

# Run program with required arguments
run: $(TARGET)
	@echo "Running server with files: $(ARGS)"
	./$(TARGET) $(ARGS)

# Clean build files
clean:
	rm -f $(TARGET) $(OBJ)

# Build and Run in one command
all: $(TARGET) run
