BIN = neve
LIB = -lmicrohttpd -luv

.PHONY: first
first:
	@echo "Run 'make all' to build the project"

.PHONY: check
check:
	cat -e -t -v  Makefile

loop/loop.p: loop/loop.cpp loop/loop.hpp
	$(CXX) -g -c loop/loop.cpp -o loop/loop.o

http/server.o: http/server.cpp http/server.hpp
	$(CXX) -g -c http/server.cpp -o http/server.o

http/path.o: http/path.cpp http/path.hpp
	$(CXX) -g -c http/path.cpp -o http/path.o

logger/logger.o: logger/logger.cpp logger/logger.hpp
	$(CXX) -g -c logger/logger.cpp -o logger/logger.o

json/parser.o: json/parser.cpp json/parser.hpp
	$(CXX) -g -c json/parser.cpp -o json/parser.o

json/printer.o: json/printer.cpp json/printer.hpp
	$(CXX) -g -c json/printer.cpp -o json/printer.o

main.o: main.cpp
	$(CXX) -g -c main.cpp

all: main.o json/printer.o json/parser.o logger/logger.o http/path.o http/server.o \
		loop/loop.o
	$(CXX) $(LIB) json/printer.o json/parser.o logger/logger.o \
		http/path.o http/server.o loop/loop.o main.o \
		-pipe -g -Wall -W -fPIC -o $(BIN)

.PHONY: clean
clean:
	rm -f $(BIN)
	rm -f *.o
	rm -f **/*.o
