#ifndef RUN_H
#define RUN_H

#include "job.hpp"
#include "../actions/sandbox.hpp"
#include "../persistence/cache.hpp"
#include "../json/parser.hpp"
#include "../logger/logger.hpp"

class Run : public Job {
public:
	Run() : Job() { }
	~Run();

	void execute();
private:
	bool parse(std::string *error);

	JSON::Parser parser;
	JSON::Value args;
	Logger logger;
};

#endif // RUN_H
