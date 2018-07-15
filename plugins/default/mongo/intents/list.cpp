#include "list.hpp"

bool IntentList::parse(JSON::Value &args) {
    if (!args["collection"].is(JSON::JSON_STRING)) {
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

bool IntentList::call(JSON::Value &args, JSON::Value *result) {
    bool success = false;

    if (!parse(args)) {
        *result = "argument error";
        return success;
    }

    std::string str = printer.print(query);
    bson_error_t err;
    bson_t queryDoc;

    // Use bson_init_from_json instead of toBson because this correctly
    // serialized the OID representation to bson
    if(!bson_init_from_json(&queryDoc, str.c_str(), str.size(), &err)) {
        *result = err.message;
        return success;
    }

    mongoc_collection_t *col = getCollection(collection.c_str());
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(col, &queryDoc,
                                                               nullptr, nullptr);

    const bson_t *doc;
    *result = JSON::Array {};

    while (mongoc_cursor_next(cursor, &doc)) {
        bson_iter_t it;
        bson_iter_init(&it, doc);

        JSON::Value item;
        item.fromBson(&it);

        result->push_back(item);
        success = true;
    }

    if (mongoc_cursor_error(cursor, &err)) {
        *result = err.message;
        success = false;
    }

    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(col);
    return success;
}
