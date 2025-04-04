CC = gcc
CFLAGS = -Wall -g
LDFLAGS = $(shell pkg-config --cflags --libs libmongoc-1.0) -lssl -lcrypto
SRC = main.c ssl.c user_handle.c log/log.c
OBJ = $(SRC:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) Web/index.html Web/script.js Web/style.css

clean:
	rm -f $(OBJ) $(TARGET)
