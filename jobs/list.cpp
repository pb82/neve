#include "list.hpp"

List::~List() { }

void List::execute() {
	JSON::Array actions;
	Cache::i().list(actions);

	this->code = 200;
	this->result = actions;
}
