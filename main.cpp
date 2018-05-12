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
HttpRouter *router = new HttpRouter;

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

	Logger logger;

	router->get("/ping", [&logger](PathParams& params, void **data) {
		logger.info("Ping request received");

		Job *job = new Job;
		job->jobType = PING;
		job->result = {{"status", "ok"}};
		*data = job;
		return 200;
	});

	loop->run(router);
    return 0;
}
