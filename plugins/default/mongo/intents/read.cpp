#include "read.hpp"

bool IntentRead::sysCall(void *in, void *out, std::string *error) {
    if (!in || !out) {
        error->append("argument error");
        return false;
    }

    // In / Out
    std::string *name = static_cast<std::string *>(in);
    Action *action = (Action *) out;

    bool success = false;
    const bson_t *doc;

    // Query action by name
    bson_t *query = bson_new();
    BSON_APPEND_UTF8(query, "name", name->c_str());


    // Get the requested collection and submit the query
    mongoc_collection_t *c = getCollection(SYS_COL_ACTIONS);
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
