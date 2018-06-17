#ifndef INTENT_LIST_H
#define INTENT_LIST_H

#include "intent.hpp"

#include "../../../../actions/action.hpp"

class IntentList : public Intent {
public:
    IntentList(mongoc_client_t *client, mongoc_database_t *db)
        : Intent(client, db) { }

    ~IntentList() { }

    bool sysCall(void *in, void *out, std::string *error);
};

#endif // INTENT_LIST_H
