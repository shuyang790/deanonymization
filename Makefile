CC = g++
CFLAG = -O0 -g -pthread
EXE = main
OBJ = main.o match.o graph.o thpool.o

$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(CFLAG)

main.o: main.cpp match.hpp 
	$(CC) -c main.cpp $(CFLAG)

match.o: match.cpp match.hpp graph.hpp thpool.hpp
	$(CC) -c match.cpp $(CFLAG)

thpool.o: thpool.cpp thpool.hpp
	$(CC) -c thpool.cpp $(CFLAG)

graph.o: graph.cpp graph.hpp
	$(CC) -c graph.cpp $(CFLAG)


clean:
	rm main *.o 
