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

    logger.debug("Action `%s' not found in cache", name.c_str());

    std::string error;
    Action *action = new Action;
    /*
    if (db->sysCall("read", &name, (void *) action, &error)) {
        // Update cache
        cached[name] = std::unique_ptr<Action>(action);
        logger.debug("Action `%s' pulled from database", name.c_str());
    } else {
        logger.error("Database read failed with error: %s", error.c_str());
        delete action;
        return nullptr;
    }
    */
    return action;
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
        {"data", JSON::Object {}}
    };

    action->toJson(payload["data"]);
    JSON::Value result = db->call("create", payload);

    if(!result["success"].as<bool>()) {
        logger.error("Database create failed for action %s", action->name);
    } else {
        logger.info("Action %s stored in database", action->name.c_str());
    }
}

void Cache::listBackend() {
    JSON::Value payload = JSON::Object {
        {"collection", SYS_COL_ACTIONS}
    };

    JSON::Value result = db->call("list", payload);
    if(!result["success"].as<bool>()) {
        logger.error("Database list failed for action");
    } else {
        JSON::Array items = result["result"].as<JSON::Array>();
        cached.clear();

        for (auto& item : items) {
            Action *action = new Action;
            action->fromJson(item);
            cached[action->name] = std::unique_ptr<Action>(action);
        }

        dirty = false;
        logger.debug("Cache repopulated successfully");
    }
}

bool Cache::deleteBackend(std::string &name) {
    /*
    std::string error;
    if (!db->sysCall("delete", &name, nullptr, &error)) {
        logger.error("Database delete failed with error: %s", error.c_str());
        return false;
    } else {
        logger.debug("Action `%s' deleted from database", name.c_str());
        return true;
    }
    */
    return false;
}

bool Cache::updateBackend(Action *action) {
    return false;
}
