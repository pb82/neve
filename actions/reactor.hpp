#ifndef REACTOR_H
#define REACTOR_H

#include <map>
#include <mutex>
#include <functional>

#include "../jobs/job.hpp"
#include "../logger/logger.hpp"

typedef std::function<void(Job *job)> ReactorCallback;

struct AsyncResult {
    Job *job;
    ReactorCallback callback;
};

class Reactor {
public:
    static Reactor &i();
    ~Reactor();

    Reactor(Reactor const&) = delete;
    void operator=(Reactor const&) = delete;

    void placeResult(Job *job);
    void placeCallback(uint uuid, ReactorCallback callback);

private:
    Reactor();
    Logger logger;

    std::map<uint, AsyncResult> items;
    std::mutex lock;
};

#endif // REACTOR_H
