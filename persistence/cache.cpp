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
	logger.info("Action %s stored in cache", action->name.c_str());
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
