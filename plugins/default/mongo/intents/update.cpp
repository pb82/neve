#include "update.hpp"

bool IntentUpdate::parse(JSON::Value &args) {
    if (!args["collection"].is(JSON::JSON_STRING)) {
        return false;
    }

    if (!args["data"].is(JSON::JSON_OBJECT)) {
        return false;
    }

    if (args["query"].is(JSON::JSON_OBJECT)) {
        query = args["query"];
    } else {
        query = JSON::Object {};
    }

    collection = args["collection"].as<std::string>();
    return true;
}

bool IntentUpdate::call(JSON::Value &args, JSON::Value *result) {
    if (!parse(args)) {
        *result = "argument error";
        return false;
    }

    std::string str = printer.print(query);
    bson_error_t err;
    bson_t queryDoc;

    // Use bson_init_from_json instead of toBson because this correctly
    // serialized the OID representation to bson
    if(!bson_init_from_json(&queryDoc, str.c_str(), str.size(), &err)) {
        *result = err.message;
        return false;
    }

    bson_t doc;
    bson_t reply;
    bson_error_t error;

    args["data"].toBson(&doc);
    mongoc_collection_t *col = getCollection(collection.c_str());
    if (!mongoc_collection_update_many(col, &queryDoc, &doc, nullptr, &reply, &error)) {
        mongoc_collection_destroy(col);
        *result = error.message;
        return false;
    }

    bson_iter_t it;
    mongoc_collection_destroy(col);
    bson_iter_init(&it, &reply);
    result->fromBson(&it);
    return true;
}
