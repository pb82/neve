#include <unistd.h>

#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"
#include "http/path.hpp"
#include "loop/loop.hpp"
#include "config/config.hpp"

Loop *loop = new Loop(new HttpRouter);
Config *config = new Config;

// Organized cleanup
void onSignal(int sig) {
	delete loop;
	delete config;
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

	config->load("./config.lua");

	loop->router()->get("/ping", [](HttpRequest *req, void **data) {
		Job *job = new Job;
		job->jobType = PING;
		job->result = JSON::Object {{"status", "ok"}};
		*data = job;
		return 200;
	});

	loop->run();
    return 0;
}
