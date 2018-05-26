#include "path.hpp"

Fragment& Fragment::operator=(const Fragment& other) {
	this->isVariable = other.isVariable;
	this->value = other.value;
	return *this;
}

void Pattern::parseQuery(std::string &path, JSON::Object &params) {
	std::string key;
	stream.str("");

	while (index < path.length()) {
		char current = path.at(index++);

		switch (current) {
		case '=':
			key = stream.str();
			stream.str("");
			break;
		case '&':
			params[key] = stream.str();
			stream.str("");
			break;
		default:
			stream << current;
		}
	}

	// If there was only one key value pair we need to set it here because
	// the loop will terminate before reaching the '&' where the pair is
	// stored
	if (key.length() > 0 && stream.str().length() > 0) {
		params[key] = stream.str();
		return;
	}

	// We also want to support the case where the query string is a flag,
	// e.g. /a/b?c
	if (stream.str().length() > 0) {
		params[stream.str()] = true;
	}
}

void Pattern::parse(std::string path, JSON::Object *params) {
	bool isVariable = false, hasQuery = false;
	stream.str("");

	while (index < path.length()) {
		char current = path.at(index++);

		// We don't deal with query parameters here
		if (current == '?') {
			hasQuery = true;
			break;
		}

		// Deal with all other characters
		switch(current) {
		case ':':
			isVariable = true;
			continue;
		case '/':
			{
				if (stream.str().length() > 0)
					fragments.push_back({isVariable, stream.str()});
				isVariable = false;
				stream.str("");
				continue;
			}
		default:
			stream << current;
		}
	}

	// Store the final fragment
	if (stream.str().length() > 0) {
		fragments.push_back({isVariable, stream.str()});
	}

	if (hasQuery && params != nullptr) {
		parseQuery(path, *params);
	}
}

bool Pattern::next(Fragment &fragment) {
	if (index >= fragments.size()) {
		return false;
	}

	// Copy-on-assignment
	fragment = fragments[index++];
	return true;
}

void Pattern::reset() {
	index = 0;
}

int Pattern::size() const {
	return fragments.size();
}

Path::Path(int method, std::string mask, MatchCallback cb)
	: method(method), cb(cb) {
	this->mask.parse(mask);
}

Path::~Path() { }

bool Path::match(HttpRequest *request) {
	request->params.clear();

	// Abort early if the methods don't match
	if (this->method != request->method) {
		return false;
	}

	Pattern ext;
	// Pass a pointer to the params because we want to store
	// the query args to should there be some
	ext.parse(request->url, &request->params);
	ext.reset();
	mask.reset();

	Fragment maskFragment;
	Fragment pathFragment;

	// Both patterns need to have the same number of
	// fragments
	if (mask.size() != ext.size()) {
		return false;
	}

	while (mask.next(maskFragment)) {
		if (!ext.next(pathFragment)) {
			return false;
		}

		if (maskFragment.isVariable) {
			request->params[maskFragment.value] = pathFragment.value;
			continue;
		}

		if (maskFragment.value.compare(pathFragment.value) != 0) {
			return false;
		}
	}

	return true;
}

int Path::invokeCallback(HttpRequest *request, void **data) {
	if (cb) {
		return cb(request, data);
	}

	return 400;
}
