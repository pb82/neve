#ifndef PATH_H
#define PATH_H

#include <functional>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "../json/value.hpp"

// Used to store path variables
typedef std::map<std::string, std::string> PathParams;

/**
 * @brief The Fragment struct
 * Represents a path fragment e.g. the ':id' in '/user/:id'.
 * A fragment can be either a variable or a constant
 */
struct Fragment {
	// Make it copyable for convenience
	Fragment& operator=(const Fragment& other);

	// Value of the fragment and a boolean indicating if it's a
	// variable name or literal
	bool isVariable = false;
	std::string value;
};

/**
 * @brief The Pattern class
 * Represents a path pattern that can contain variables.
 * Variables are fragments that start with a colon, e.g.
 * '/user/:id' matches all paths that consist of '/user/'
 * plus another param which will be saved in a map under
 * 'id'
 */
class Pattern {
public:
	Pattern() { }

	void parse(std::string path);

	/**
	 * @brief next Read the next fragment, incrementing the index
	 * @param fragment A reference to a fragment that will be filled
	 * with the values of the next fragment
	 * @return true if there is a next fragment
	 */
	bool next(Fragment &fragment);

	/**
	 * @brief reset Reset index to zero
	 */
	void reset();
private:
	std::ostringstream stream;
	std::vector<Fragment> fragments;
	int index = 0;
};

/**
 * @brief The Path class
 * Represents a path portion of a url, e.g.
 * the '/ping' in 'http://localhost:8080/ping'
 */
class Path {
public:
	Path(const char *method, std::string mask);
	~Path();

	/**
	 * @brief match Matches the given path against the pattern and stores
	 * all path variables it encounters in the 'vars' argument.
	 * @param path The Http Method of the request
	 * @param path The path to match against
	 * @param vars (optional) The path params map
	 * @return true if the path matches the pattern
	 */
	bool match(std::string method, std::string path, PathParams &&vars = {});
private:
	std::string method;
	Pattern mask;
};

#endif // PATH_H

