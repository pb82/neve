#ifndef INTENT_DELETE_H
#define INTENT_DELETE_H

#include "intent.hpp"

#include "../../../../actions/action.hpp"

class IntentDelete : public Intent {
public:
    IntentDelete(mongoc_client_t *client, mongoc_database_t *db)
        : Intent(client, db) { }

    ~IntentDelete() { }

    bool call(JSON::Value &args, JSON::Value *result);

private:
    bool parse(JSON::Value &args);
    std::string collection;
    JSON::Value query;
    JSON::Printer printer;
};

#endif // INTENT_DELETE_H
