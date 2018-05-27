#include "plugin_mongo.hpp"

std::string PluginMongo::name() const {
	return PLUGIN_NAME;
}

PluginMongo::~PluginMongo() {
	if (db) mongoc_database_destroy(db);
	if (client) mongoc_client_destroy(client);
}

void PluginMongo::start() {
	// For now we're using non pooled connection mode
	client = mongoc_client_new(connectionString.c_str());
	if (!client) {
		throw PluginError("failed to create mongo client");
	}

	db = mongoc_client_get_database(client, database.c_str());
}

void PluginMongo::configure(JSON::Value &config) {
	if (!config.is(JSON::JSON_OBJECT)) {
		throw PluginError("config missing or invalid");
	}

	if (!config["connectionString"].is(JSON::JSON_STRING)) {
		throw PluginError("`connectionString' missing or invalid");
	}
	connectionString = config["connectionString"].as<std::string>();

	if (!config["database"].is(JSON::JSON_STRING)) {
		throw PluginError("`database' missing or invalid");
	}
	database = config["database"].as<std::string>();
}

JSON::Value PluginMongo::call(const std::string &intent, JSON::Value &args) {
	return JSON::null;
}
