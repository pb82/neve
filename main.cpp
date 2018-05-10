#include <iostream>
#include <string>
#include <cstring>

#include <microhttpd.h>
#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"
#include "http/path.hpp"
#include "loop/loop.hpp"

Loop *loop = new Loop;

void onSignal(int sig) {
	delete loop;
	exit(sig);
}

void connectSignals() {
	signal(SIGABRT, onSignal);
	signal(SIGTERM, onSignal);
	signal(SIGINT,  onSignal);
	signal(SIGSEGV, onSignal);
}

int main() {
	connectSignals();
	loop->run();
    return 0;
}
