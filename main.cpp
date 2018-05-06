#include <iostream>
#include <string>
#include <cstring>

#include <microhttpd.h>
#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"
#include "http/path.hpp"
#include "http/server.hpp"

#define PORT 8080

int main() {
	Logger logger;
	HttpServer server(PORT);
	int count = 0;

	server.get("/ping", [&count](DoneCallback done) {
		count++;
		done();
	});

	server.run();
	getchar();

	std::cout << "Requests received: " << count << std::endl;
    return 0;
}
