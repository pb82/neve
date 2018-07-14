#include "uuid.hpp"

namespace UUID {
    // TODO: create a real UUID instead of just a random number
    std::string create() {
        bson_oid_t oid;
        bson_oid_init(&oid, nullptr);

        char oidString[25];
        bson_oid_to_string(&oid, oidString);
        return oidString;
    }
}
