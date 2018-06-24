#ifndef ACTION_H
#define ACTION_H

#include <string>

#include "../json/value.hpp"

/**
 * @brief The Action struct
 * Represents an action as it's stored in the cache/database
 */
struct Action {
    std::string name;
    std::string bytecode;
    int timeout = 0;
    int memory = 0;
    int size = 0;

    void toJson(JSON::Value &target) {
        target["name"] = name;
        target["size"] = size;
        target["memory"] = memory;
        target["timeout"] = timeout;
        target["bytecode"] = JSON::Value(bytecode.c_str(),
                                         bytecode.size());
    }
};

#endif // ACTION_H
