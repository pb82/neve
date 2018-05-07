#include "loop.hpp"

std::queue<Job *> Loop::jobs;
std::mutex Loop::lock;
uv_loop_t *Loop::loop = uv_default_loop();

Loop::Loop() {
	uv_idle_init(loop, &idler);
	uv_idle_start(&idler, loopIdle);
}

Loop::~Loop() {
	uv_idle_stop(&idler);
	uv_loop_close(loop);
}

void Loop::enqueue(Job * const job) {
	std::lock_guard<std::mutex> l(lock);
	jobs.push(job);
}

Job *const Loop::dequeue() {
	Job *result = nullptr;
	if (jobs.size() > 0) {
		result = jobs.front();
		jobs.pop();
	}
	return result;
}

void Loop::loopIdle(uv_idle_t *) {
	Job *next = dequeue();
	if (next) {
		submit(next);
	} else {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Loop::run() const {
	uv_run(loop, UV_RUN_DEFAULT);
}

void Loop::submit(Job *const job) {
	std::lock_guard<std::mutex> l(lock);
	job->req.data = (void *) job;
	uv_queue_work(loop, &job->req, jobRun, jobDone);
}

void Loop::jobRun(uv_work_t *req) {
	Job *const job = static_cast<Job *>(req->data);
	switch(job->type) {
	case NOP:
		break;
	case PING:
		job->result = "OK";
		break;
	default:
		break;
	}
}

void Loop::jobDone(uv_work_t *req, int status) {
	Job *const job = static_cast<Job *>(req->data);
	job->callback(&job->result);
	delete job;
}
