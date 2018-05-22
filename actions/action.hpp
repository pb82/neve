#ifndef ACTION_H
#define ACTION_H

#include <string>

/**
 * @brief The Action struct
 * Represents an action as it's stored in the cache/database
 */
struct Action {
	std::string name;
	std::string bytecode;
	int timeout = 0;
	int memory = 0;
	int size = 0;
};

#endif // ACTION_H
