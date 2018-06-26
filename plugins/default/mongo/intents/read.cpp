#include "read.hpp"

bool IntentRead::parse(JSON::Value &args) {
    if (!args["collection"].is(JSON::JSON_STRING)) {
        return false;
    }

    if (!args["id"].is(JSON::JSON_STRING)) {
        return false;
    }

    collection = args["collection"].as<std::string>();
    id = args["id"].as<std::string>();
    return true;
}

bool IntentRead::call(JSON::Value &args, JSON::Value *result) {
    if (!parse(args)) {
        *result = "argument error";
        return false;
    }

    args["query"] = JSON::Object {{
        "_id", id
    }};

    args["query"]["_id"].toBsonOid();

    IntentList list(client, db);

    JSON::Value results;
    if (list.call(args, &results)) {
        *result = results.as<JSON::Array>()[0];
        return true;
    }

    return false;
}
