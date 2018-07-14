#include "update.hpp"

Update::~Update() { }

void Update::execute() {
    JSON::Value payload;

    // Parse payload
    try {
        parser.parse(payload, body.c_str());
    } catch(JSON::ParseError err) {
        this->result = err.what();
        this->code = 400;
        return;
    }

    // Check if a name parameter is present, required to identify the action
    if(params.find("id") == params.end()) {
        this->result = "argument error";
        this->code = 400;
        return;
    }
    std::string name = params["id"].as<std::string>();

    // Get the action from the cache or database
    Action *currentAction = Cache::i().read(name);
    if (!currentAction) {
        this->result = "not found";
        this->code = 404;
        return;
    }

    // Set new timeout
    if (payload["timeout"].is(JSON::JSON_NUMBER)) {
        int newTimeout = payload["timeout"].as<int>();
        newTimeout = newTimeout >= 0 ? newTimeout : 0;
        currentAction->timeout = newTimeout;
    }

    // Set new memory restriction
    if (payload["memory"].is(JSON::JSON_NUMBER)) {
        int newMemory = payload["memory"].as<int>();
        newMemory = newMemory >= 0 ? newMemory : 0;
        currentAction->memory = newMemory;
    }

    // Compile the new source code and update the bytecode
    if (payload["code"].is(JSON::JSON_STRING)) {
        std::string source = payload["code"].as<std::string>();

        Compiler compiler(source.c_str());

        // Decode the base64 string and compile the source code
        // if successful
        bool result = compiler.decode();
        if (!result) {
            this->result = "decode error";
            this->code = 400;
            return;
        }

        result = compiler.compile();
        if (!result) {
            this->result = "compile error";
            this->code = 400;
            return;
        }

        currentAction->bytecode = compiler.getBytecode();
        currentAction->size = currentAction->bytecode.size();
    }

    bool success = Cache::i().update(currentAction);
    this->result = success ? "OK" : "update error";
    this->code = success ? 200 : 400;
}
