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
    if (!parse(args)) {
        *result = "argument error";
        return false;
    }

    bson_t queryDoc;
    const bson_t *doc;
    query.toBson(&queryDoc);


    // Get the requested collection and submit the query
    mongoc_collection_t *col = getCollection(collection.c_str());
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(col, &queryDoc,
                                                               nullptr, nullptr);

    *result = JSON::Array {};

    while (mongoc_cursor_next(cursor, &doc)) {
        bson_iter_t it;
        bson_iter_init(&it, doc);

        JSON::Value item;
        item.fromBson(&it);
        result->push_back(item);
    }

    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(col);
    return true;
}
