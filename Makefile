BIN = neve
LIB = -luv -llua

.PHONY: first
first:
	@echo "Run 'make all' to build the project"

.PHONY: check
check:
	cat -e -t -v  Makefile

vendor/http_parser.o: vendor/http_parser.c vendor/http_parser.h
	$(CXX) -g -c vendor/http_parser.c -o vendor/http_parser.o

loop/loop.o: loop/loop.cpp loop/loop.hpp
	$(CXX) -g -c loop/loop.cpp -o loop/loop.o

http/path.o: http/path.cpp http/path.hpp
	$(CXX) -g -c http/path.cpp -o http/path.o

http/router.o: http/router.cpp http/router.hpp
	$(CXX) -g -c http/router.cpp -o http/router.o

http/response.o: http/response.cpp http/response.hpp
	$(CXX) -g -c http/response.cpp -o http/response.o

logger/logger.o: logger/logger.cpp logger/logger.hpp
	$(CXX) -g -c logger/logger.cpp -o logger/logger.o

json/value.o: json/value.cpp json/value.hpp
	$(CXX) -g -c json/value.cpp -o json/value.o

json/parser.o: json/parser.cpp json/parser.hpp
	$(CXX) -g -c json/parser.cpp -o json/parser.o

json/printer.o: json/printer.cpp json/printer.hpp
	$(CXX) -g -c json/printer.cpp -o json/printer.o

config/config.o: config/config.cpp config/config.hpp
	$(CXX) -g -c config/config.cpp -o config/config.o

jobs/ping.o: jobs/ping.cpp jobs/ping.cpp
	$(CXX) -g -c jobs/ping.cpp -o jobs/ping.o

main.o: main.cpp
	$(CXX) -g -c main.cpp

all: main.o json/printer.o json/parser.o logger/logger.o http/path.o \
		loop/loop.o vendor/http_parser.o http/router.o http/response.o \
		json/value.o jobs/ping.o config/config.o
	$(CXX) $(LIB) json/printer.o json/parser.o logger/logger.o \
		http/path.o http/router.o loop/loop.o vendor/http_parser.o \
		json/value.o jobs/ping.o config/config.o \
		http/response.o main.o -pipe -g -Wall -W -fPIC -o $(BIN)

.PHONY: clean
clean:
	rm -f $(BIN)
	rm -f *.o
	rm -f **/*.o
