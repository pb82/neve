#include "router.hpp"

void HttpRouter::get(const char *route, MatchCallback cb) {
	registerHandler(GET, route, cb);
}

void HttpRouter::post(const char *route, MatchCallback cb) {
	registerHandler(POST, route, cb);
}

void HttpRouter::registerHandler(int method, const char *route, MatchCallback cb) {
	Path *path = new Path(method, route, cb);
	routes.push_back(std::unique_ptr<Path>(path));
}

int HttpRouter::run(HttpRequest *request, void **data) const {
	for (const auto& route: routes) {
		if (route.get()->match(request)) {
			return route.get()->invokeCallback(request, data);
		}
	}

	// If no route matches return 404 - Not found
	*data = nullptr;
	return 404;
}
