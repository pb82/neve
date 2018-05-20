#ifndef CREATE_H
#define CREATE_H

#include "job.hpp"
#include "../json/parser.hpp"
#include "../json/printer.hpp"
#include "../actions/compiler.hpp"

class Create : public Job {
public:
	Create() : Job() { }
	~Create();

	void execute();
private:
	bool parse(std::string *error);

	JSON::Parser parser;
	JSON::Value payload;

	// Parsed properties
	std::string source;
	std::string name;
	int timeout = 0;
	int memory = 0;
};

#endif // CREATE_H
