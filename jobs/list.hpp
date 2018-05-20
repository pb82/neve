#ifndef LIST_H
#define LIST_H

#include "job.hpp"
#include "../persistence/cache.hpp"

class List : public Job {
public:
	List() : Job() { }
	~List();

	void execute();
private:
};

#endif // LIST_H
