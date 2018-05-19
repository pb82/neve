#include <unistd.h>

#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"
#include "http/path.hpp"
#include "loop/loop.hpp"
#include "config/config.hpp"

// Globals
Loop *loop = nullptr;
Config *config = nullptr;

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
	// Cleanup handlers
	connectSignals();

	config = new Config;
	config->load("./config.lua");

	loop = new Loop(config->get("server"), new HttpRouter);

	// Configuration
	Logger::configure(config->get("logger"));
	Logger l;

	// Http endpoints
	loop->router()->get("/ping", [](HttpRequest *req, void **data) {
		Job *job = new Job;
		job->jobType = PING;
		*data = job;
		return 200;
	});

	loop->router()->post("/actions", [&l](HttpRequest *req, void **) {
		l.info("Body: %s", req->body.c_str());
		return 200;
	});

	// Start server
	loop->run();
	return 0;
}
