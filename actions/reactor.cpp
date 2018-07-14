#include "reactor.hpp"

Reactor::Reactor() { }

Reactor::~Reactor() { }

Reactor &Reactor::i() {
    static Reactor instance;
    return instance;
}

void Reactor::placeResult(Job *job) {
    std::lock_guard<std::mutex> guard(lock);
    uint uuid = job->getUUID();

    // A callback is already registered. Invoke it immediately
    if (items.find(uuid) != items.end()) {
        AsyncResult &result = items[uuid];
        result.callback(job);
        items.erase(uuid);
        return;
    }

    AsyncResult result;
    result.job = job;
    items[uuid] = result;
    logger.debug("Async job with uuid %d finished", uuid);
}

void Reactor::placeCallback(uint uuid, ReactorCallback callback) {
    std::lock_guard<std::mutex> guard(lock);

    // A job is already registered. Invoke the callback immediately
    if (items.find(uuid) != items.end()) {
        AsyncResult &result = items[uuid];
        callback(result.job);
        items.erase(uuid);
        return;
    }

    AsyncResult result;
    result.callback = callback;
    items[uuid] = result;
    logger.debug("Callback placed for job with uuid %d", uuid);
}
