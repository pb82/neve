#include "plugin_mongo.hpp"

std::string PluginMongo::name() const {
    return PLUGIN_NAME;
}

PluginMongo::~PluginMongo() {
    if (db) mongoc_database_destroy(db);
    if (client) mongoc_client_destroy(client);
    intents.clear();
}

void PluginMongo::globalInit() {
    mongoc_init();
}

void PluginMongo::globalCleanup() {
    mongoc_cleanup();
}

void PluginMongo::start() {
    // For now we're using non-pooled connection mode
    client = mongoc_client_new(connectionString.c_str());
    if (!client) {
        throw PluginError("failed to create mongo client");
    }

    db = mongoc_client_get_database(client, database.c_str());

    // CRUDL handlers
    intents["create"]   = std::unique_ptr<Intent>(new IntentCreate(client, db));
    intents["read"]     = std::unique_ptr<Intent>(new IntentRead(client, db));
    intents["update"]   = std::unique_ptr<Intent>(new IntentUpdate(client, db));
    intents["delete"]   = std::unique_ptr<Intent>(new IntentDelete(client, db));
    intents["list"]     = std::unique_ptr<Intent>(new IntentList(client, db));
}

void PluginMongo::configure(JSON::Value &config) {
    if (!config.is(JSON::JSON_OBJECT)) {
        throw PluginError("config missing or invalid");
    }

    // Get the connection string
    if (!config["connectionString"].is(JSON::JSON_STRING)) {
        throw PluginError("`connectionString' missing or invalid");
    }
    connectionString = config["connectionString"].as<std::string>();

    // Get the database that this plugin instance is allowed
    // to use
    if (!config["database"].is(JSON::JSON_STRING)) {
        throw PluginError("`database' missing or invalid");
    }
    database = config["database"].as<std::string>();
}

JSON::Value PluginMongo::call(const std::string &intent, JSON::Value &args) {
    // TODO: use the mongo connection pool to get rid of this lock
    std::lock_guard<std::mutex> lock(mutex);

    if (intents.find(intent) == intents.end()) {
        return JSON::Object {
            {"success", false},
            {"result", "unknown intent"}
        };
    }

    JSON::Value result;
    bool success = intents[intent].get()->call(args, &result);

    // The form of the reply is the same for every intent.
    // If success is false then result will contain the error message
    // Otherwise it will contain whatver the intent returned
    return JSON::Object {
        {"success", success},
        {"result", result}
    };
}
