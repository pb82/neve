#ifndef JOB_H
#define JOB_H

#include <uv.h>

#include "../http/request.hpp"
#include "../json/value.hpp"

/**
 * @brief The Job class
 * Base class for all jobs that are executed on one of the
 * worker threads
 */
class Job {
public:
    Job() {
        workRequest.data = this;
    }

    virtual ~Job() { }

    /**
     * @brief execute Run the job
     * Must be implemented by classes inheriging from this class.
     * Shoud set code and result
     */
    virtual void execute() = 0;

    int getCode() const {
        return this->code;
    }

    void setHttpRequest(HttpRequest *const req) {
        this->httpRequest = req;
    }

    HttpRequest *getHttpRequest() {
        return httpRequest;
    }

    uv_work_t *getWorkRequest() {
        return &workRequest;
    }

    JSON::Value& getResult() {
        return result;
    }

    bool getBlock() {
        return this->block;
    }

    void setBlock(bool value) {
        this->block = value;
    }

    uint getUUID() {
        return this->uuid;
    }

    void setUUID(uint value) {
        this->uuid = value;
    }

protected:
    uv_work_t workRequest;

    // Incoming HTTP request to be consumed by execute
    HttpRequest *httpRequest;

    // HTTP result and code to be set by execute
    JSON::Value result;
    int code = 500;

    // Wait for result or return immediately
    bool block = true;
    uint uuid;
};

#endif // JOB_H
