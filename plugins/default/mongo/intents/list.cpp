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
    mongoc_collection_t *col = getCollection(SYS_COL_ACTIONS);
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(col, &queryDoc,
                                                               nullptr, nullptr);



}

/*
bool IntentList::sysCall(void *in, void *out, std::string *error) {
    const bson_t *doc;
    bson_t *query = bson_new();

    std::vector<Action *>* actions = (std::vector<Action *>*) out;

    // Get the requested collection and submit the query
    mongoc_collection_t *c = getCollection(SYS_COL_ACTIONS);
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(c, query, nullptr, nullptr);

    while (mongoc_cursor_next(cursor, &doc)) {
        bson_iter_t it;
        bson_iter_init(&it, doc);

        Action *action = new Action;

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
        actions->push_back(action);
    }

    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(c);
    bson_destroy(query);
    return true;
}
*/
