#ifndef LOOP_H
#define LOOP_H

#include <uv.h>
#include <thread>
#include <functional>
#include <queue>
#include <iostream>
#include <mutex>

#include "../json/value.hpp"

enum JobType {
	NOP		= 1,
	PING	= 2
};

struct Job {
	JobType type = NOP;
	JSON::Value payload;
	JSON::Value result;
	std::function<void(JSON::Value *result)> callback;
	uv_work_t req;
};

class Loop {
public:
	Loop();
	~Loop();

	void run() const;

	static void submit(Job *const job);
	static void enqueue(Job *const job);
	static Job *const dequeue();
private:
	static void jobRun(uv_work_t *req);
	static void jobDone(uv_work_t *req, int status);
	static void loopIdle(uv_idle_t *);

	static uv_loop_t *loop;
	uv_idle_t idler;

	static std::queue<Job *> jobs;
	static std::mutex lock;
};

#endif // LOOP_H

