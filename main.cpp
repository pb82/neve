#include <unistd.h>

#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"
#include "http/path.hpp"
#include "loop/loop.hpp"
#include "jobs/ping.hpp"
#include "jobs/create.hpp"
#include "jobs/list.hpp"
#include "jobs/run.hpp"
#include "config/config.hpp"
#include "persistence/cache.hpp"
#include "plugins/registry.hpp"

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

void setupRoutes() {
	// Http endpoints
	loop->router()->get("/ping", [](HttpRequest *req, void **data) {
		*data = new Ping;
		return true;
	});

	loop->router()->post("/action", [](HttpRequest *req, void **data) {
		*data = new Create;
		return true;
	});

	loop->router()->post("/action/:id", [](HttpRequest *req, void **data) {
		*data = new Run;
		return true;
	});

	loop->router()->get("/action", [](HttpRequest *req, void **data) {
		*data = new List;
		return true;
	});
}

bool tryLoadPersistence() {
	// If the config file does not have a persistence section then
	// just return here. Actions will not be persisted but the cache
	// will still work
	if (!config->has("persistence")) return false;

	JSON::Value persistenceConfig = config->get("persistence");

	if (!persistenceConfig["path"].is(JSON::JSON_STRING)) {
		return false;
	}
	std::string path = persistenceConfig["path"].as<std::string>();
	PluginRegistry::i().newInstance("skeleton", path);
	// PluginRegistry::i().cleanup();
	return true;
}

int main() {
	// Cleanup handlers
	connectSignals();

	config = new Config;
	config->load("./config.lua");

	// Set global log level
	Logger::configure(config->get("logger"));

	// Try to load the mongo plugin and set up persistence
	tryLoadPersistence();

	// Setup the event loop & http router
	loop = new Loop(config->get("server"), new HttpRouter);
	setupRoutes();

	// Start server
	loop->initTcp();
	loop->run();
	return 0;
}
