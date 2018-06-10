#include "plugin_mongo.hpp"

std::string PluginMongo::name() const {
    return PLUGIN_NAME;
}

PluginMongo::~PluginMongo() {
    if (db) mongoc_database_destroy(db);
    if (client) mongoc_client_destroy(client);
    mongoc_cleanup();
}

void PluginMongo::start() {
    // For now we're using non-pooled connection mode
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
    JSON::Object response = {{"success", false}};

    if (intent.compare("create") == 0) {
        if (!args["collection"].is(JSON::JSON_STRING)) {
            throw PluginError("`collection' must be a string");
        }
        std::string collection = args["collection"].as<std::string>();

        if (!args["data"].is(JSON::JSON_OBJECT)) {
            throw PluginError("`data' must be an object");
        }

        JSON::Value result;
        response["success"] = create(collection, args["data"], &result);
        response["result"] = result;
        return response;
    }

    response["result"] = "Unkown intent";
    return response;
}

bool PluginMongo::create(std::string collection, JSON::Value &data, JSON::Value *result) {
    std::string json = printer.print(data);
    bson_error_t error;

    // Convert the data from JSON to BSON
    bson_t *doc = bson_new_from_json((const uint8_t*) json.c_str(), json.size(), &error);
    if (!doc) {
        *result = error.message;
        return false;
    }

    // Set _id
    bson_oid_t oid;
    bson_oid_init(&oid, nullptr);
    BSON_APPEND_OID(doc, "_id", &oid);

    // Get the requested collection
    mongoc_collection_t *c = mongoc_client_get_collection(client, database.c_str(),
        collection.c_str());

    // Insert the document
    if(!mongoc_collection_insert_one(c, doc, nullptr, nullptr, &error)) {
        mongoc_collection_destroy(c);
        bson_destroy(doc);

        *result = error.message;
        return false;
    }


    // Obtain the oid string
    char oidString[25];
    bson_oid_to_string(&oid, oidString);
    *result = oidString;

    // Cleanup resources
    mongoc_collection_destroy(c);
    bson_destroy(doc);

    return true;
}

bool PluginMongo::sysCall(std::string intent, void *in, void **out, std::string *error) {
    if(intent.compare("storeAction") == 0) {
        return storeAction((Action *) in, error);
    }

    if (intent.compare("deleteAction") == 0) {
        return deleteAction((Action *) in, error);
    }

    if (intent.compare("readAction") == 0) {
        return readAction((std::string *) in, out, error);
    }

    return false;
}

bool PluginMongo::readAction(std::string *name, void **out, std::string *error) {
    bool success = false;
    const bson_t *doc;

    // Query action by name
    bson_t *query = bson_new();
    BSON_APPEND_UTF8(query, "name", name->c_str());

    Action *action = new Action;
    *out = action;

    // Get the requested collection and submit the query
    mongoc_collection_t *c = mongoc_client_get_collection(client, database.c_str(),
        "actions");

    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(c, query, nullptr, nullptr);

    while (mongoc_cursor_next(cursor, &doc)) {
        bson_iter_t it;
        bson_iter_init(&it, doc);

        bson_iter_find(&it, "name");
        action->name = bson_iter_utf8(&it, nullptr);

        bson_iter_init(&it, doc);
        bson_iter_find(&it, "size");
        action->size = bson_iter_int32(&it);

        bson_iter_init(&it, doc);
        bson_iter_find(&it, "timeout");
        action->timeout = bson_iter_int32(&it);

        bson_iter_init(&it, doc);
        bson_iter_find(&it, "memory");
        action->memory = bson_iter_int32(&it);

        bson_iter_init(&it, doc);
        bson_iter_find(&it, "bytecode");
        const char *buffer;
        uint32_t len;
        bson_iter_binary(&it, nullptr, &len, (const uint8_t **) &buffer);
        std::string bytecode(buffer, len);
        action->bytecode = bytecode;
        success = true;
    }

    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(c);
    bson_destroy(query);
    return success;
}

bool PluginMongo::storeAction(Action *data, std::string *error) {
    bson_oid_t oid;
    bson_error_t err;
    bson_t *doc = bson_new();
    bool success = false;

    bson_oid_init(&oid, nullptr);
    BSON_APPEND_OID(doc, "_id", &oid);
    BSON_APPEND_UTF8(doc, "name", data->name.c_str());
    BSON_APPEND_INT32(doc, "timeout", data->timeout);
    BSON_APPEND_INT32(doc, "memory", data->memory);
    BSON_APPEND_INT32(doc, "size", data->size);
    BSON_APPEND_BINARY(doc, "bytecode", BSON_SUBTYPE_BINARY,
        (const uint8_t *) data->bytecode.c_str(), data->size
    );

    // Get the actions collection
    mongoc_collection_t *c = mongoc_client_get_collection(client,
        database.c_str(), "actions");

    if (!mongoc_collection_insert_one(c, doc, nullptr, nullptr, &err)) {
        error->append(err.message);
    } else {
        ensureIndex("actions", "name");
        success = true;
    }

    mongoc_collection_destroy(c);
    bson_destroy(doc);
    return success;
}

bool PluginMongo::deleteAction(Action *data, std::string *error) {
    bson_error_t err;
    bool success = false;
    bson_t *query = bson_new();

    // Get the actions collection
    mongoc_collection_t *c = mongoc_client_get_collection(client,
        database.c_str(), "actions");

    BSON_APPEND_UTF8(query, "name", data->name.c_str());
    if (!mongoc_collection_delete_many(c, query, nullptr, nullptr, &err)) {
        error->append(err.message);
    } else {
        success = true;
    }

    mongoc_collection_destroy(c);
    bson_destroy(query);
    return success;
}

void PluginMongo::ensureIndex(const char *col, const char *prop) {
    bson_t keys;
    bson_error_t err;
    bson_init(&keys);
    BSON_APPEND_INT32(&keys, prop, 1);

    char *idx = mongoc_collection_keys_to_index_string(&keys);

    // http://mongoc.org/libmongoc/current/create-indexes.html
    // https://docs.mongodb.com/manual/reference/command/createIndexes/
    bson_t *cmd = BCON_NEW(
        "createIndexes",
        BCON_UTF8(col),
        "indexes",
        "[",
        "{",
        "key",
        BCON_DOCUMENT(&keys),
        "name",
        BCON_UTF8(idx),
        "unique",
        BCON_BOOL(true),
        "}",
        "]"
    );

    mongoc_database_write_command_with_opts(db, cmd, nullptr, nullptr, &err);
    bson_destroy(cmd);
    bson_free(idx);
}
