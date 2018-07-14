#include "reactor.hpp"

Reactor::Reactor() { }

Reactor::~Reactor() { }

Reactor &Reactor::i() {
    static Reactor instance;
    return instance;
}

void Reactor::placeResult(Job *job) {
    std::lock_guard<std::mutex> guard(lock);
    std::string uuid = job->getUUID();

    // A callback is already registered. Invoke it immediately
    if (items.find(uuid) != items.end()) {
        AsyncResult &result = items[uuid];
        result.callback(result.code, result.result);
        items.erase(uuid);
        return;
    }

    AsyncResult result;
    result.code = job->getCode();
    result.result = job->getResult();
    items[uuid] = result;
    logger.debug("Async job with uuid %s finished", uuid.c_str());
}

void Reactor::placeCallback(std::string uuid, ReactorCallback callback) {
    std::lock_guard<std::mutex> guard(lock);

    // A job is already registered. Invoke the callback immediately
    if (items.find(uuid) != items.end()) {
        AsyncResult &result = items[uuid];
        callback(result.code, result.result);
        items.erase(uuid);
        return;
    }

    AsyncResult result;
    result.callback = callback;
    items[uuid] = result;
    logger.debug("Callback placed for job with uuid %s", uuid.c_str());
}
