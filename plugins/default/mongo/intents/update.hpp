#ifndef INTENT_UPDATE_H
#define INTENT_UPDATE_H

#include <iostream>

#include "intent.hpp"

class IntentUpdate : public Intent {
public:
    IntentUpdate(mongoc_client_t *client, mongoc_database_t *db)
        : Intent(client, db) { }

    ~IntentUpdate() { }

    bool call(JSON::Value &args, JSON::Value *result);

private:
    bool parse(JSON::Value &args);
    std::string collection;
    JSON::Value query;
};

#endif // INTENT_UPDATE_H
