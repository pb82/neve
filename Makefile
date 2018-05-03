BIN = neve

main.o: main.cpp
	$(CXX) -c main.cpp

build: main.o
	$(CXX) main.o -o $(BIN)

clean:
	rm -f $(BIN)
	rm -f *.o
