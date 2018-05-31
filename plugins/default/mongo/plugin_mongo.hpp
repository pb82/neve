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
	mongoc_client_t *client = nullptr;
	mongoc_database_t *db = nullptr;
};

// Creates the entrypoint for the server to create an instance
// of the plugin
EXPORT_PLUGIN(PluginMongo)

#endif // PLUGIN_MONGO_H
