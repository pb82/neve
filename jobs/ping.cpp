#include "ping.hpp"

Ping::~Ping() { }

void Ping::execute() {
	this->code = 200;
	this->result = "OK";
}
