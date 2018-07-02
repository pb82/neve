#include "cache.hpp"

Cache::Cache() { }
Cache &Cache::i() {
    static Cache instance;
    return instance;
}

Cache::~Cache() {
    cached.clear();
}

void Cache::store(Action *action) {
    std::lock_guard<std::mutex> guard(lock);
    cached[action->name] = std::unique_ptr<Action>(action);
    if (db) storeBackend(action);
}

bool Cache::remove(std::string &name) {
    std::lock_guard<std::mutex> guard(lock);
    cached.erase(name);
    return deleteBackend(name);
}

bool Cache::update(Action *action) {
    std::lock_guard<std::mutex> guard(lock);
    return updateBackend(action);
}

Action *Cache::read(std::string &name) {
    if (cached.find(name) != cached.end()) {
        return cached[name].get();
    }

    logger.debug("Action %s not found in cache", name.c_str());

    JSON::Value payload = JSON::Object {
        {"collection", SYS_COL_ACTIONS},
        {"query", JSON::Object {{"name", name}}}
    };

    JSON::Value result = db->call("list", payload);

    if (!result["success"].as<bool>()) {
        logger.error("Database read failed");
        return nullptr;
    } else if (!result["result"].is(JSON::JSON_ARRAY)) {
        logger.error("Unexpected result type");
        return nullptr;
    } else {
        try {
            JSON::Array results = result["result"].as<JSON::Array>();
            if (results.size() != 1) {
                logger.error("Unexpected number of results: %d", results.size());
                return nullptr;
            }

            Action *action = new Action;
            action->fromJson(results.at(0));
            cached[action->name] = std::unique_ptr<Action>(action);
            return action;
        } catch (std::runtime_error err) {
            logger.error(err.what());
            return nullptr;
        }
    }
}

void Cache::list(JSON::Array &actions) {
    if (dirty) {
        listBackend();
    } else {
        logger.debug("Listing actions from cache");
    }

    Actions::iterator iter;
    for (iter = cached.begin(); iter != cached.end(); iter++) {
        const Action& action = *iter->second;

        // Every action is represented as a JSON object
        actions.push_back(JSON::Object {
            {"name",	action.name},
            {"size",	action.size},
            {"memory",	action.memory},
            {"timeout",	action.timeout}
        });
    }
}

void Cache::storeBackend(Action *action) {
    JSON::Value payload = JSON::Object {
        {"collection", SYS_COL_ACTIONS},
        {"data", JSON::Object {}},
        {"unique", {"name"}}
    };

    action->toJson(payload["data"]);
    JSON::Value result = db->call("create", payload);

    if(!result["success"].as<bool>()) {
        logger.error("%s", printer.print(result).c_str());
    } else {
        logger.debug("Action `%s' stored in database", action->name.c_str());
    }
}

void Cache::listBackend() {
    JSON::Value payload = JSON::Object {
        {"collection", SYS_COL_ACTIONS}
    };

    JSON::Value result = db->call("list", payload);
    if(!result["success"].as<bool>()) {
        logger.error("%s", printer.print(result).c_str());
    } else {
        JSON::Array items = result["result"].as<JSON::Array>();
        cached.clear();

        for (auto& item : items) {
            Action *action = new Action;
            action->fromJson(item);
            cached[action->name] = std::unique_ptr<Action>(action);
        }

        logger.debug("Cache repopulated from database");
        dirty = false;
    }
}

bool Cache::deleteBackend(std::string &name) {
    JSON::Value payload = JSON::Object {
        {"collection", SYS_COL_ACTIONS},
        {"query", JSON::Object {{"name", name}}}
    };

    JSON::Value result = db->call("delete", payload);

    if(!result["success"].as<bool>()) {
        logger.error("%s", printer.print(result).c_str());
        return false;
    } else {
        logger.debug("%s", printer.print(result).c_str());
        return true;
    }
}

bool Cache::updateBackend(Action *action) {
    JSON::Value updated;
    action->toJson(updated);

    JSON::Value payload = JSON::Object {
        {"collection", SYS_COL_ACTIONS},
        {"query", JSON::Object {{"name", action->name}}},
        {"data", JSON::Object {{ "$set", updated }}}
    };

    JSON::Value result = db->call("update", payload);

    if(!result["success"].as<bool>()) {
        logger.error("%s", printer.print(result).c_str());
        return false;
    } else {
        logger.debug("%s", printer.print(result).c_str());
        return true;
    }
}
