#ifndef PING_H
#define PING_H

#include "job.hpp"

/**
 * @brief The Ping class
 * Implements server ping. Does nothing other than setting the code
 * to 200 and the result to "OK"
 */
class Ping : public Job {
public:
	Ping() : Job() { }
	~Ping();

	void execute();
private:
};

#endif // PING_H
