#ifndef CACHE_H
#define CACHE_H

#include <memory>
#include <string>
#include <mutex>
#include <map>

#include "../json/value.hpp"
#include "../json/printer.hpp"
#include "../logger/logger.hpp"
#include "../actions/action.hpp"
#include "../http/base64.hpp"
#include "../plugins/plugin.hpp"

typedef std::map<std::string, std::unique_ptr<Action>> Actions;

class Cache {
public:
	static Cache &i();
	~Cache();

	Cache(Cache const&) = delete;
	void operator=(Cache const&) = delete;



	void store(Action *action);
	void list(JSON::Array &actions);
	void remove(std::string &name);
	Action *read(std::string &name);

	void setPersistencePlugin(Plugin *plugin) {
		this->db = plugin;
	}

private:
	Cache();

	void storeBackend(Action *action);

	// Action definitions stored in memory
	Actions cached;
	Logger logger;
	JSON::Printer printer;

	// Points to the persistence plugin (default is
	// mongodb). If not set actions are only cached
	// in memory
	Plugin *db = nullptr;

	// Some actions of the cache need to be synchronized
	std::mutex lock;
};

#endif // CACHE_H
