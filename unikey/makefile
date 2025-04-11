CC = g++
CFLAGS = -Wall 
LIBS = -lX11 -lXi -lXtst
SRC = unikey.cpp
TARGET = unikey

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

run : all 
	./$(TARGET)

clean: 
	rm -rf $(TARGET)