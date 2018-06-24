#ifndef INTENT_READ_H
#define INTENT_READ_H

#include "intent.hpp"

#include "../../../../actions/action.hpp"

class IntentRead : public Intent {
public:
    IntentRead(mongoc_client_t *client, mongoc_database_t *db)
        : Intent(client, db) { }

    ~IntentRead() { }
};

#endif // INTENT_READ_H