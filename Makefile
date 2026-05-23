CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = expression_eval
SRC      = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
