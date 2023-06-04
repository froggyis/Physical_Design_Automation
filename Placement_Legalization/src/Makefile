CC = g++
OPT = -O3
LIBS = -std=c++17

all: main

main: main.cpp
	$(CC) -o ../bin/hw4 main.cpp ./Parser/Parser.cpp ./Legalizer/Legalizer.cpp $(OPT) $(LIBS)

clean:
	rm -f ../bin/hw4
	rm -f ../output/*