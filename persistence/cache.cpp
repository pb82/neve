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

void Cache::storeBackend(Action *action) {
    std::string error;
    if(!db->sysCall("storeAction", action, nullptr, &error)) {
        logger.error("Error storing action `%s' in database", error.c_str());
    } else {
        logger.debug("Action `%s' stored in database", action->name.c_str());
    }
}

void Cache::remove(std::string &name) {
    std::lock_guard<std::mutex> guard(lock);
    cached.erase(name);
}

Action *Cache::read(std::string &name) {
    if (cached.find(name) != cached.end()) {
        return cached[name].get();
    }

    logger.debug("Action `%s'not found in cache", name.c_str());

    std::string error;
    Action *action = nullptr;
    if (db->sysCall("readAction", &name, (void **) &action, &error)) {
        // Update cache
        cached[name] = std::unique_ptr<Action>(action);
        logger.debug("Action `%s' pulled from database", name.c_str());
    } else {
        logger.error("Action `%s' not found in database", name.c_str());
        if (action) delete action;
        return nullptr;
    }

    return action;
}

void Cache::list(JSON::Array &actions) {
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
