#include "create.hpp"

Create::~Create() { }

bool Create::parse(std::string *error) {
	try {
		parser.parse(payload, getHttpRequest()->body.c_str());
	} catch(JSON::ParseError err) {
		error->append(err.what());
		return false;
	}

	if (!payload.is(JSON::JSON_OBJECT)) {
		error->append("Payload needs to be an object");
		return false;
	}

	// Check code
	if (!payload["code"].is(JSON::JSON_STRING)) {
		error->append("`code' missing or invalid");
		return false;
	}
	this->source = payload["code"].as<std::string>();

	// Check name
	if (!payload["name"].is(JSON::JSON_STRING)) {
		error->append("`name' missing or invalid");
		return false;
	}
	this->name = payload["name"].as<std::string>();

	// Check payload
	if (!payload["timeout"].is(JSON::JSON_NUMBER)) {
		error->append("`timeout' missing or invalid");
		return false;
	}
	this->timeout = payload["timeout"].as<int>();

	// Check memory
	if (!payload["memory"].is(JSON::JSON_NUMBER)) {
		error->append("`memory' missing or invalid");
		return false;
	}
	this->memory = payload["memory"].as<int>();
	return true;
}

void Create::store(std::string &bytecode) const {
	Action *action = new Action;

	action->size = bytecode.size();
	action->bytecode = bytecode;
	action->timeout = timeout;
	action->memory = memory;
	action->name = name;

	// The cache is supposed to sync to a persistent
	// backend at some point
	Cache::i().store(action);
}

void Create::execute() {
	// Try to parse the request payload
	std::string error;
	if(!parse(&error)) {
		this->code = 400;
		this->result = error;
		return;
	}

	Compiler compiler(source.c_str());
	bool result = false;

	// Decode the base64 string and compile the source code
	// if successful
	result = compiler.decode();
	if (result) {
		result = compiler.compile();
		store(compiler.getBytecode());
	}

	if (result) {
		this->code = 200;
		this->result = "OK";
	} else {
		this->code = 400;
		this->result = compiler.getError();
	}
}
