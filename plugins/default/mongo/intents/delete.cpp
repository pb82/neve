#include "delete.hpp"
/*
bool IntentDelete::sysCall(void *in, void *, std::string *error) {
    if (!in) {
        error->append("argument error");
        return false;
    }

    std::string *name = static_cast<std::string *>(in);

    bson_error_t err;
    bool success = false;
    bson_t *query = bson_new();

    // Get the actions collection
    mongoc_collection_t *c = getCollection(SYS_COL_ACTIONS);

    BSON_APPEND_UTF8(query, "name", name->c_str());
    if (!mongoc_collection_delete_many(c, query, nullptr, nullptr, &err)) {
        error->append(err.message);
    } else {
        success = true;
    }

    mongoc_collection_destroy(c);
    bson_destroy(query);
    return success;
}
*/
