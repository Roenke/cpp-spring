BIN = ./bin/
SRC = ./src/

OBJS = $(BIN)main.o
TARGET = ./bin/linked_ptr
CXXFLAGS = -std=c++11 -Wall -Werror -g -O2

all: bin build

build: $(OBJS) 
	g++ $(OBJS) $(CXXFLAGS) -o $(TARGET)

$(BIN)%.o: $(SRC)%.cpp
	g++ -c $< $(CXXFLAGS) -o $@

bin:
	mkdir -p bin

memcheck: all
	valgrind --tool=memcheck --leak-check=full $(TARGET)

clean:
	rm -r $(BIN)*
