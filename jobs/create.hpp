#ifndef CREATE_H
#define CREATE_H

#include "job.hpp"

class Create : public Job {
public:
	Create() : Job() { }
	~Create();

	void execute();
private:
};

#endif // CREATE_H
