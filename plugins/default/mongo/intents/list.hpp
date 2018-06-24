#ifndef INTENT_LIST_H
#define INTENT_LIST_H

#include "intent.hpp"

#include "../../../../actions/action.hpp"

class IntentList : public Intent {
public:
    IntentList(mongoc_client_t *client, mongoc_database_t *db)
        : Intent(client, db) { }

    ~IntentList() { }

    bool call(JSON::Value &args, JSON::Value *result);
private:

    bool IntentList::parse(JSON::Value &args);
    std::string collection;
    JSON::Value query;
};

#endif // INTENT_LIST_H
