CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lX11 -lXi -lXtst

SRC = test.cpp map.cpp handleKeyboard.cpp daemon.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = unikey

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

run: $(TARGET)
	./$(TARGET)
