#include "create.hpp"

bool IntentCreate::parse(JSON::Value &args) {
    if (!args["collection"].is(JSON::JSON_STRING)) {
        return false;
    }

    if (!args["data"].is(JSON::JSON_OBJECT)) {
        return false;
    }

    collection = args["collection"].as<std::string>();
    data = printer.print(args["data"]);
    return true;
}

bool IntentCreate::call(JSON::Value &args, JSON::Value *result) {
    if (!parse(args)) {
        *result = "argument error";
        return false;
    }

    bson_error_t error;

    // Convert the data from JSON to BSON
    bson_t *doc = bson_new_from_json((const uint8_t*) data.c_str(), data.size(), &error);
    if (!doc) {
        *result = error.message;
        return false;
    }

    // Set _id
    bson_oid_t oid;
    bson_oid_init(&oid, nullptr);
    BSON_APPEND_OID(doc, "_id", &oid);

    mongoc_collection_t *col = getCollection(collection.c_str());

    // Insert the document
    if(!mongoc_collection_insert_one(col, doc, nullptr, nullptr, &error)) {
        mongoc_collection_destroy(col);
        bson_destroy(doc);

        *result = error.message;
        return false;
    }


    // Obtain the oid string
    char oidString[25];
    bson_oid_to_string(&oid, oidString);
    *result = oidString;

    // Cleanup resources
    mongoc_collection_destroy(col);
    bson_destroy(doc);
    return true;
}

bool IntentCreate::sysCall(void *in, void *, std::string *error) {
    if (!in) {
        *error = "argument error";
        return false;
    }

    Action *data = static_cast<Action *>(in);

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
    mongoc_collection_t *c = getCollection(SYS_COL_ACTIONS);

    if (!mongoc_collection_insert_one(c, doc, nullptr, nullptr, &err)) {
        error->append(err.message);
    } else {
        ensureUnique(SYS_COL_ACTIONS, "name");
        success = true;
    }

    mongoc_collection_destroy(c);
    bson_destroy(doc);
    return success;
}
