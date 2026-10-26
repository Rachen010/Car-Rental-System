CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Iinclude
SRC      = main.cpp $(wildcard src/*.cpp)
TARGET   = car_rental

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
