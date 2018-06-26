#ifndef INTENT_READ_H
#define INTENT_READ_H

#include "list.hpp"

#include "../../../../actions/action.hpp"

class IntentRead : public Intent {
public:
    IntentRead(mongoc_client_t *client, mongoc_database_t *db)
        : Intent(client, db) { }

    ~IntentRead() { }

    bool call(JSON::Value &args, JSON::Value *result);
private:
    bool parse(JSON::Value &args);
    std::string collection;
    std::string id;
};

#endif // INTENT_READ_H
