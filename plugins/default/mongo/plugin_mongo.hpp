#ifndef PLUGIN_MONGO_H
#define PLUGIN_MONGO_H

#include <libmongoc-1.0/mongoc.h>

#include "../../plugin.hpp"

#define PLUGIN_NAME "mongo"

/**
 * @brief The PluginMongo class
 * A Mongodb plugin that can be used by actions to make CRUDLA (Create, Read
 * Update, Delete, List, Aggregate). Will also be used by the server itself
 * to provide persistence for the admin db.
 */
class PluginMongo : public Plugin {
public:
	PluginMongo() { }

	// Interface implementation
	~PluginMongo();
	std::string name() const;
	void configure(JSON::Value &config);
	void start();
	JSON::Value call(const std::string &intent, JSON::Value &args);

private:
	// Parsed from the config
	std::string connectionString;
	std::string database;

	// Mongodb connection object, will exist during the lifetime
	// of the plugin
	mongoc_client_t *client;
	mongoc_database_t *db;
};

/**
 * @brief newInstance Returns a new instance of the plugin
 * This needs to be exported as a C function to avoid name mangling. Plugins
 * should support many instances, e.g. the mongo plugin will be used for
 * general storage as well as for the admin DB itself
 */
extern "C" Plugin *newInstance() {
	return new PluginMongo;
}

#endif // PLUGIN_MONGO_H
