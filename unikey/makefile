CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lX11 -lXi -lXtst -lunistring 

SRC = main.cpp map.cpp handleLogic/handleKeyboard.cpp config/daemon.cpp config/config.cpp handleLogic/handleXJSRF.cpp handleLogic/handleTelexTransform.cpp
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
