#ifndef CACHE_H
#define CACHE_H

#include <memory>
#include <string>
#include <mutex>
#include <map>

#include "../json/value.hpp"
#include "../logger/logger.hpp"

struct Action;
typedef std::map<std::string, std::unique_ptr<Action>> Actions;

struct Action {
	std::string name;
	std::string bytecode;
	int timeout = 0;
	int memory = 0;
	int size = 0;
};

class Cache {
public:
	static Cache &i();
	~Cache();

	Cache(Cache const&) = delete;
	void operator=(Cache const&) = delete;

	void store(Action *action);
	void list(JSON::Array &actions);

private:
	Cache();

	// Action definitions stored in memory
	Actions cached;
	Logger logger;

	// Some actions of the cache need to be synchronized
	std::mutex lock;
};

#endif // CACHE_H
