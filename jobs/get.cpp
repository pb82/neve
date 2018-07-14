#include "get.hpp"

Get::~Get() { }

bool Get::parse(std::string *error) {
    if (params.find("id") == params.end()) {
        error->append("Action id missing");
        return false;
    }
    this->id = params["id"].as<std::string>();
    return true;
}

void Get::execute() {
    std::string error;
    if (!parse(&error)) {
        result = error;
        code = 400;
        return;
    }

    Action *action = Cache::i().read(id);
    if (!action) {
        result = "Not found";
        code = 404;
        return;
    }

    result = JSON::Object {
        {"name", action->name},
        {"size", action->size},
        {"timeout", action->timeout},
        {"memory", action->memory},
    };

    this->code = 200;
}
