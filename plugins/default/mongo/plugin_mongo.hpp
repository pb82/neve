#ifndef PLUGIN_MONGO_H
#define PLUGIN_MONGO_H

#include <libmongoc-1.0/mongoc.h>

#include "../../plugin.hpp"
#include "../../../json/printer.hpp"
#include "../../../actions/action.hpp"

#include <iostream>

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

	bool sysCall(std::string intent, void *data, std::string *error);

private:
	bool create(std::string collection, JSON::Value &data, JSON::Value *result);

	/**
	 * @brief storeAction Store an action in the database
	 * Used by sysCall to store a given action in the database including it's
	 * bytecode in binary format.
	 * @param data A pointer to the action to store
	 * @param error A pointer to a string where the error message will be stored
	 * should there be one
	 * @return true if the action is stored successfully
	 */
	bool storeAction(Action *data, std::string *error);

	/**
	 * @brief ensureIndex Creates an index
	 * This function currently always creates an index for the given collection
	 * and property. It does not check whether the index already exists or not.
	 * @param col Collection to create the index in
	 * @param property The property the index should apply to
	 */
	void ensureIndex(const char *col, const char *prop);

	// Parsed from the config
	std::string connectionString;
	std::string database;

	// Most interactions with Mongo involve json
	JSON::Printer printer;

	// Mongodb connection object, will exist during the lifetime
	// of the plugin
	mongoc_client_t *client = nullptr;
	mongoc_database_t *db = nullptr;
};

// Creates the entrypoint for the server to create an instance
// of the plugin
EXPORT_PLUGIN(PluginMongo)

#endif // PLUGIN_MONGO_H
