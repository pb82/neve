#ifndef PING_H
#define PING_H

#include "job.hpp"

class Ping : public Job {
public:
	Ping() : Job() { }
	~Ping();

	void execute();
private:
};

#endif // PING_H
