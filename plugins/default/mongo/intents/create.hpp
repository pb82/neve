#ifndef INTENT_CREATE_H
#define INTENT_CREATE_H

#include "intent.hpp"

#include "../../../../actions/action.hpp"

class IntentCreate : public Intent {
public:
    IntentCreate(mongoc_client_t *client, mongoc_database_t *db)
        : Intent(client, db) { }

    ~IntentCreate() { }

    bool call(JSON::Value &args, JSON::Value *result);

private:
    bool parse(JSON::Value &args);
    std::string collection;
};

#endif // INTENT_CREATE_H
