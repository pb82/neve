BIN = neve
LIB = -lmicrohttpd -luv

.main.o: main.cpp
	$(CXX) -c main.cpp

all: main.o
	$(CXX) $(LIB) main.o -o $(BIN)

clean:
	rm -f $(BIN)
	rm -f *.o
