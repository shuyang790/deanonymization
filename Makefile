CC = g++
CFLAG = -O3
EXE = main
OBJ = main.o match.o graph.o

$(EXE): $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(CFLAG)

main.o: main.cpp match.hpp 
	$(CC) -c main.cpp $(CFLAG)

match.o: match.cpp match.hpp graph.hpp
	$(CC) -c match.cpp $(CFLAG)

graph.o: graph.cpp graph.hpp
	$(CC) -c graph.cpp $(CFLAG)


clean:
	rm main *.o
