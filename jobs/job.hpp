#ifndef JOB_H
#define JOB_H

#include <uv.h>

#include "../http/request.hpp"
#include "../json/value.hpp"

class Job {
public:
	Job() {
		workRequest.data = this;
	}

	virtual ~Job() { }
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

	const JSON::Value& getResult() {
		return result;
	}

protected:
	uv_work_t workRequest;

	// Incoming HTTP request to be consumed by execute
	HttpRequest *httpRequest;

	// HTTP result and code to be set by execute
	JSON::Value result;
	int code = 500;
};

#endif // JOB_H
