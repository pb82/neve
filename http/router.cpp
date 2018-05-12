#include "router.hpp"

void HttpRouter::get(const char *route, MatchCallback cb) {
	Path *path = new Path(GET, route, cb);
	routes.push_back(std::unique_ptr<Path>(path));
}

int HttpRouter::run(int method, std::string& path, void **data) {
	PathParams vars;

	for (const auto& route: routes) {
		if (route.get()->match(method, path, vars)) {
			return route.get()->invokeCallback(vars, data);
		}
	}

	// If no route matches return 404 - Not found
	*data = nullptr;
	return 404;
}
