#ifndef PLUGIN_MONGO_H
#define PLUGIN_MONGO_H

#include <libmongoc-1.0/mongoc.h>
#include <memory>
#include <map>

#include "../../plugin.hpp"
#include "../../../json/printer.hpp"
#include "../../../actions/action.hpp"

#include "intents/create.hpp"
#include "intents/read.hpp"
#include "intents/delete.hpp"
#include "intents/list.hpp"

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

    bool sysCall(std::string intent, void *in, void *out, std::string *error);

private:
    // call handlers
    bool create(std::string collection, JSON::Value &data, JSON::Value *result);

    // sysCall handlers
    bool storeAction(Action *data, std::string *error);
    bool deleteAction(Action *data, std::string *error);
    bool readAction(std::string *name, void *out, std::string* error);
    bool listActions(void *out, std::string* error);

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

    std::map<std::string, std::unique_ptr<Intent>> intents;
};

// Creates the entrypoint for the server to create an instance
// of the plugin
EXPORT_PLUGIN(PluginMongo)

#endif // PLUGIN_MONGO_H
