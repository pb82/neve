#ifndef ROUTER_H
#define ROUTER_H

#include <map>
#include <vector>
#include <memory>

#include "path.hpp"

class HttpRouter {
public:
	/**
	 * @brief get Register a GET request handler
	 * @param route The path patterh to register the handler for
	 * @param cb The callback invoked when the pattern matches the
	 * current path
	 */
	void get(const char *route, MatchCallback cb);

	/**
	 * @brief run Run the router and try to match the given path agains
	 * all patterns
	 * @param method Http method
	 * @param path URL
	 * @param data A pointer to a pointer that can be initialized with a
	 * Job to be run on the queue
	 * @return An integer representing an Http status code
	 */
	int run(int method, std::string &path, void **data);
private:		
	std::vector<std::unique_ptr<Path>> routes;
};

#endif // ROUTER_H

