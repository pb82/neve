#include "path.hpp"

Fragment& Fragment::operator=(const Fragment& other) {
	this->isVariable = other.isVariable;
	this->value = other.value;
	return *this;
}

void Pattern::parse(std::string path) {
	bool isVariable = false;
	ss.str("");

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
				if (ss.str().length() > 0)
					fragments.push_back({ss.str(), isVariable});
				isVariable = false;
				ss.str("");
				continue;
			}
		default:
			ss << current;
		}
	}

	// Store the final fragment
	if (ss.str().length() > 0) {
		fragments.push_back({ss.str(), isVariable});
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

Path::Path(const char *method, std::string mask) : method(method) {
	this->mask.parse(mask);
}

Path::~Path() { }

bool Path::match(std::string method, std::string path, PathParams &&vars) {
	vars.clear();

	// Abort early if the methods don't match
	if (this->method.compare(method) != 0) {
		return false;
	}

	Pattern ext;
	ext.parse(path);
	ext.reset();
	mask.reset();

	Fragment maskFragment;
	Fragment pathFragment;

	while (mask.next(maskFragment)) {
		if (!ext.next(pathFragment)) {
			return false;
		}

		if (maskFragment.isVariable) {
			vars[maskFragment.value] = pathFragment.value;
			continue;
		}

		if (maskFragment.value.compare(pathFragment.value) != 0) {
			return false;
		}
	}

	return true;
}
