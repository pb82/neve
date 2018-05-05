BIN = neve
LIB = -lmicrohttpd -luv

check:
	cat -e -t -v  Makefile

json/parser.o: json/parser.cpp json/parser.hpp
	$(CXX) -c json/parser.cpp -o json/parser.o

json/printer.o: json/printer.cpp json/printer.hpp
	$(CXX) -c json/printer.cpp -o json/printer.o

main.o: main.cpp
	$(CXX) -c main.cpp

all: main.o json/printer.o json/parser.o
	$(CXX) $(LIB) json/printer.o json/parser.o main.o -o $(BIN)

clean:
	rm -f $(BIN)
	rm -f *.o
	rm -f **/*.o
