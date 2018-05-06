#include <iostream>
#include <string>
#include <cstring>

#include <microhttpd.h>
#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"
#include "http/path.hpp"
#include "http/server.hpp"
#include "loop/loop.hpp"

#define PORT 8080

static Loop *loop = new Loop;
// static HttpServer *server = new HttpServer(PORT);

void onSignal(int sig) {
	//delete server;
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
	/*
	server->get("/ping", [](DoneCallback done) {
		Job *job = new Job;
		job->type = PING;
		job->callback = done;
		loop->enqueue(job);
	});

	server->run();
	*/
	loop->run();

    return 0;
}
