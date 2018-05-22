#ifndef RUN_H
#define RUN_H

#include "job.hpp"
#include "../actions/sandbox.hpp"

class Run : public Job {
public:
	Run() : Job() { }
	~Run();

	void execute();
private:

};

#endif // RUN_H
