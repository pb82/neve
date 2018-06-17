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
    bool sysCall(void *in, void *, std::string *error);

private:
    bool parse(JSON::Value &args);

    // The collection parsed from the arguments object
    std::string collection;
    std::string data;
};

#endif // INTENT_CREATE_H
