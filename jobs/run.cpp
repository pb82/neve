#include "run.hpp"

Run::~Run() { }

bool Run::parse(std::string *error) {
    if (params.find("id") == params.end()) {
        error->append("Action id missing");
        return false;
    }

    // If no payload is sent, default the script arguments to an
    // empty object
    if (body.size() == 0) {
        args = JSON::Object{};
    } else {
        try {
            // Otherwise try to parse the provided payload and
            // use it as script args
            parser.parse(args, body);
        } catch(JSON::ParseError err) {
            error->append(err.what());
            return false;
        }
    }

    return true;
}

void Run::execute() {
    std::string error;
    if (!parse(&error)) {
        result = error;
        code = 400;
        return;
    }

    // Get the requested action from the cache and create a Sandbox
    // around it
    std::string name = params["id"].as<std::string>();
    Action *action = Cache::i().read(name);
    if (!action) {
        result = "No such action";
        code = 404;
        return;
    }

    Sandbox sandbox(action);

    // Run the action in the sandbox
    switch(sandbox.run(args, result, &error)) {
    case Success:
        code = 200;
        break;
    default:
        result = error;
        code = 400;
        break;
    }
}
