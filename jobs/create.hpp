#ifndef CREATE_H
#define CREATE_H

#include "job.hpp"
#include "../json/parser.hpp"
#include "../json/printer.hpp"
#include "../actions/compiler.hpp"

/**
 * @brief The Create class
 * Handles creation of new actions. Parses the request payload and
 * compiles the source code to bytecode
 */
class Create : public Job {
public:
	Create() : Job() { }
	~Create();

	void execute();
private:
	/**
	 * @brief parse Parse payload
	 * Parses and validates the JSON payload
	 * @param error Set if the payload contains invalid values
	 * @return true if the payload is valid
	 */
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
