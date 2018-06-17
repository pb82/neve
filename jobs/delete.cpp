#include "delete.hpp"

Delete::~Delete() { }

bool Delete::parse(std::string *error) {
    if (getHttpRequest()->params.find("id") == getHttpRequest()->params.end()) {
        error->append("Action id missing");
        return false;
    }
    this->id = getHttpRequest()->params["id"].as<std::string>();
    return true;
}

void Delete::execute() {
    std::string error;
    if (!parse(&error)) {
        result = error;
        code = 400;
        return;
    }

    bool success = Cache::i().remove(id);
    if (!success) {
        this->code = 404;
        this->result = "No action deleted";
    } else {
        this->code = 200;
        this->result = "OK";
    }
}
