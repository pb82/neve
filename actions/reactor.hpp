#ifndef REACTOR_H
#define REACTOR_H

#include <map>
#include <mutex>
#include <functional>

#include "../jobs/job.hpp"
#include "../logger/logger.hpp"

struct AsyncResult;
typedef std::function<void(int code, JSON::Value &result)> ReactorCallback;

struct AsyncResult {
    int code;
    JSON::Value result;
    ReactorCallback callback;
};

class Reactor {
public:
    static Reactor &i();
    ~Reactor();

    Reactor(Reactor const&) = delete;
    void operator=(Reactor const&) = delete;

    void placeResult(Job *job);
    void placeCallback(std::string uuid, ReactorCallback callback);

private:
    Reactor();
    Logger logger;

    std::map<std::string, AsyncResult> items;
    std::mutex lock;
};

#endif // REACTOR_H
