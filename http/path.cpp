#include "path.hpp"

Fragment& Fragment::operator=(const Fragment& other) {
	this->isVariable = other.isVariable;
	this->value = other.value;
	return *this;
}

void Pattern::parse(std::string path) {
	bool isVariable = false;
	stream.str("");

	while (index < path.length()) {
		char current = path.at(index++);

		// We don't deal with query parameters here
		if (current == '?') break;

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
	ext.parse(request->url);
	ext.reset();
	mask.reset();

	Fragment maskFragment;
	Fragment pathFragment;

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
