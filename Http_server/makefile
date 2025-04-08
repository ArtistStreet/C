CC = gcc
CFLAGS = -Wall -g $(shell pkg-config --cflags libmongoc-1.0)
LDFLAGS = $(shell pkg-config --libs libmongoc-1.0) -lssl -lcrypto
SRC = main.c https/ssl.c user_handle/user_handle.c log/log.c load_folder/load_folder.c
OBJ = $(SRC:.c=.o)
TARGET = main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET) Web

clean:
	rm -f $(OBJ) $(TARGET)
