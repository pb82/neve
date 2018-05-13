#include <iostream>
#include <string>
#include <cstring>

#include <microhttpd.h>
#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"
#include "http/path.hpp"
#include "loop/loop.hpp"

Loop *loop = new Loop(new HttpRouter);

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

	JSON::Value val;
	val.fromLua(nullptr);

	loop->router()->get("/ping", [&logger](HttpRequest *req, void **data) {
		Job *job = new Job;
		job->jobType = PING;
		job->result = JSON::Object {{"status", "ok"}};
		*data = job;
		return 200;
	});

	loop->run();
    return 0;
}
