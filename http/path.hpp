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

// The callback invoked when a path matches
typedef std::function<int(PathParams& params, void **data)> MatchCallback;

enum HttpMethod {
	GET		= 1,
	POST	= 3
};

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
	Path(int method, std::string mask, MatchCallback cb);
	~Path();

	/**
	 * @brief match Matches the given path against the pattern and stores
	 * all path variables it encounters in the 'vars' argument.
	 * @param path The Http Method of the request
	 * @param path The path to match against
	 * @param vars (optional) The path params map
	 * @return true if the path matches the pattern
	 */
	bool match(int method, std::string path, PathParams &vars);

	/**
	 * @brief invokeCallback Invokes the callback assigned to this pattern. This
	 * should only be called if match returned true.
	 * @param params A reference to a map containing the params read from the path
	 * @param data A pointer to a pointer that can be initialized with the data to
	 * be put on the job queue.
	 * @return An integer representing the http status code to return to the client
	 */
	int invokeCallback(PathParams& params, void **data);
private:	
	int method;
	Pattern mask;

	// Optional callback to be invoked when that patter matches
	// agains a provided URL
	MatchCallback cb;
};

#endif // PATH_H

