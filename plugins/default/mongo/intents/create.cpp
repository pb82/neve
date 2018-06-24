#include "create.hpp"

bool IntentCreate::parse(JSON::Value &args) {
    if (!args["collection"].is(JSON::JSON_STRING)) {
        return false;
    }

    if (!args["data"].is(JSON::JSON_OBJECT)) {
        return false;
    }

    collection = args["collection"].as<std::string>();
    return true;
}

bool IntentCreate::call(JSON::Value &args, JSON::Value *result) {
    if (!parse(args)) {
        *result = "argument error";
        return false;
    }

    bson_t doc;
    bson_error_t error;
    args["data"].toBson(&doc);

    std::string oid;
    setOid(&doc, oid);

    mongoc_collection_t *col = getCollection(collection.c_str());

    // Insert the document
    if(!mongoc_collection_insert_one(col, &doc, nullptr, nullptr, &error)) {
        mongoc_collection_destroy(col);
        *result = error.message;
        return false;
    }

    // Cleanup resources
    mongoc_collection_destroy(col);
    bson_destroy(&doc);
    *result = oid;
    return true;
}
