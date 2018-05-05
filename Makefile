BIN = neve
LIB = -lmicrohttpd -luv

check:
	cat -e -t -v  Makefile

logger/logger.o: logger/logger.cpp logger/logger.hpp
	$(CXX) -c logger/logger.cpp -o logger/logger.o

json/parser.o: json/parser.cpp json/parser.hpp
	$(CXX) -c json/parser.cpp -o json/parser.o

json/printer.o: json/printer.cpp json/printer.hpp
	$(CXX) -c json/printer.cpp -o json/printer.o

main.o: main.cpp
	$(CXX) -c main.cpp

all: main.o json/printer.o json/parser.o logger/logger.o
	$(CXX) $(LIB) json/printer.o json/parser.o logger/logger.o main.o -o $(BIN)

clean:
	rm -f $(BIN)
	rm -f *.o
	rm -f **/*.o
