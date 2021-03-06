#include <unistd.h>
#include <iostream>

#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"
#include "http/path.hpp"
#include "loop/loop.hpp"
#include "jobs/ping.hpp"
#include "jobs/create.hpp"
#include "jobs/list.hpp"
#include "jobs/run.hpp"
#include "jobs/get.hpp"
#include "jobs/delete.hpp"
#include "jobs/update.hpp"
#include "config/config.hpp"
#include "persistence/cache.hpp"
#include "plugins/registry.hpp"

#include "jobs/uuid.hpp"

// Globals
Loop *loop = nullptr;

// Organized cleanup
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

void setupRoutes() {
    // Http endpoints
    loop->router()->get("/ping", [](HttpRequest *req, void **data) {
        *data = new Ping;
        return RT_Sync;
    });

    loop->router()->post("/action", [](HttpRequest *req, void **data) {
        *data = new Create;
        return RT_Sync;
    });

    loop->router()->post("/action/:id", [](HttpRequest *req, void **data) {
        *data = new Run;

        // Check if the --block flag is set
        if (req->params.find("block") != req->params.end()) return RT_Sync;
        else return RT_Async;
    });

    loop->router()->get("/action/:id", [](HttpRequest *req, void **data) {
        *data = new Get;
        return RT_Sync;
    });

    loop->router()->put("/action/:id", [](HttpRequest *req, void **data) {
        *data = new Update;
        return RT_Sync;
    });

    loop->router()->get("/action", [](HttpRequest *req, void **data) {
        *data = new List;
        return RT_Sync;
    });

    loop->router()->del("/action/:id", [](HttpRequest *req, void **data) {
        *data = new Delete;
        return RT_Sync;
    });

    loop->router()->get("/resolve/:token", [](HttpRequest *req, void **) {
        if (req->params.find("token") == req->params.end()) return RT_Error;
        else return RT_Delayed;
    });
}

bool tryLoadPersistence() {
    // If the config file does not have a persistence section then
    // just return here. Actions will not be persisted but the cache
    // will still work
    if (!Config::i().has("persistence")) return false;
    JSON::Value persistenceConfig = Config::i().get("persistence");

    if (!persistenceConfig["path"].is(JSON::JSON_STRING)) {
        return false;
    }
    std::string path = persistenceConfig["path"].as<std::string>();
    Plugin *db = PluginRegistry::i().newInstance("mongo", path, persistenceConfig);
    Cache::i().setPersistencePlugin(db);
    return true;
}

int main() {
    // Cleanup handlers
    connectSignals();

    Config::i().load("./config.lua");

    // Set global log level
    Logger::configure(Config::i().get("logger"));
    Logger logger;

    PluginRegistry::i().loadFromConfig();

    // Try to load the mongo plugin and set up persistence
    if(!tryLoadPersistence()) {
        logger.warn("No persistence provider loaded");
    }

    // Setup the event loop & http router
    loop = new Loop(Config::i().get("server"), new HttpRouter);
    setupRoutes();

    // Start server
    loop->initTcp();
    loop->run();
    return 0;
}
