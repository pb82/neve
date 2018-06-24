#ifndef INTENT_H
#define INTENT_H

#include <libmongoc-1.0/mongoc.h>
#include <string>

#include "../../../../json/value.hpp"
#include "../../../../json/printer.hpp"

class Intent {
public:
    Intent(mongoc_client_t *client, mongoc_database_t *db)
        : client(client), db(db) { }

    virtual ~Intent() = default;
    /**
     * @brief call from action to plugin
     * Empty default implementation because some actions may be
     * system / action context only
     * @param args arguments as JSON
     * @return result as JSON
     */
    virtual bool call(JSON::Value &args, JSON::Value *result) {
        *result = "not implemented";
        return false;
    }

protected:
    // Get collection by name
    mongoc_collection_t *getCollection(const char *name) {
        return mongoc_client_get_collection(client,
                    mongoc_database_get_name(db), name);
    }

    /**
     * @brief setOid Sets the _id field of a document
     * Creates a new ObjectID and sets it as the _id field of a
     * BSON document
     * @param doc The document to set the ID on
     * @param result A reference to a string. The generated ObjectID will
     * be appended to that string.
     */
    void setOid(bson_t *doc, std::string &result) {
        bson_oid_t oid;
        bson_oid_init(&oid, nullptr);
        BSON_APPEND_OID(doc, "_id", &oid);

        char oidString[25];
        bson_oid_to_string(&oid, oidString);
        result.append(oidString);
    }

    /**
     * @brief ensureUnique Create unique index
     * Creates a unique index if none already exists for the given collection
     * and property
     * @param col Collection to create the index for
     * @param prop Property to create the index for
     */
    void ensureUnique(const char *col, const char *prop) {
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

    mongoc_client_t *client = nullptr;
    mongoc_database_t *db = nullptr;

    JSON::Printer printer;
};

#endif // INTENT_H
