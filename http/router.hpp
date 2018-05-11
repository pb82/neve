#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <memory>

#include "path.hpp"

class HttpRouter {
public:
private:
	std::vector<std::unique_ptr<Path>> routes;
};

#endif // ROUTER_H

