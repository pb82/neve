#ifndef INTENT_DELETE_H
#define INTENT_DELETE_H

#include "intent.hpp"

#include "../../../../actions/action.hpp"

class IntentDelete : public Intent {
public:
    IntentDelete(mongoc_client_t *client, mongoc_database_t *db)
        : Intent(client, db) { }

    ~IntentDelete() { }

    bool sysCall(void *in, void *, std::string *error);
};

#endif // INTENT_DELETE_H
