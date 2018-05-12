BIN = neve
LIB = -luv

.PHONY: first
first:
	@echo "Run 'make all' to build the project"

.PHONY: check
check:
	cat -e -t -v  Makefile

http-parser/http_parser.o: http-parser/http_parser.c http-parser/http_parser.h
	$(MAKE) -C http-parser

loop/loop.o: loop/loop.cpp loop/loop.hpp
	$(CXX) -g -c loop/loop.cpp -o loop/loop.o

http/path.o: http/path.cpp http/path.hpp
	$(CXX) -g -c http/path.cpp -o http/path.o

http/router.o: http/router.cpp http/router.hpp
	$(CXX) -g -c http/router.cpp -o http/router.o

logger/logger.o: logger/logger.cpp logger/logger.hpp
	$(CXX) -g -c logger/logger.cpp -o logger/logger.o

json/parser.o: json/parser.cpp json/parser.hpp
	$(CXX) -g -c json/parser.cpp -o json/parser.o

json/printer.o: json/printer.cpp json/printer.hpp
	$(CXX) -g -c json/printer.cpp -o json/printer.o

main.o: main.cpp
	$(CXX) -g -c main.cpp

all: main.o json/printer.o json/parser.o logger/logger.o http/path.o \
		loop/loop.o http-parser/http_parser.o http/router.o
	$(CXX) $(LIB) json/printer.o json/parser.o logger/logger.o \
		http/path.o http/router.o loop/loop.o http-parser/http_parser.o \
		main.o -pipe -g -Wall -W -fPIC -o $(BIN)

.PHONY: clean
clean:
	rm -f $(BIN)
	rm -f *.o
	rm -f **/*.o
