#include "router.hpp"

HttpRouter::~HttpRouter() {
    routes.clear();
}

void HttpRouter::get(const char *route, MatchCallback cb) {
    registerHandler(GET, route, cb);
}

void HttpRouter::post(const char *route, MatchCallback cb) {
    registerHandler(POST, route, cb);
}

void HttpRouter::del(const char *route, MatchCallback cb) {
    registerHandler(DELETE, route, cb);
}

void HttpRouter::put(const char *route, MatchCallback cb) {
    registerHandler(PUT, route, cb);
}

void HttpRouter::registerHandler(int method, const char *route, MatchCallback cb) {
    Path *path = new Path(method, route, cb);
    routes.push_back(std::unique_ptr<Path>(path));
}

RunType HttpRouter::run(HttpRequest *request, void **data) const {
    // Check every route to find one that matches
    for (const auto& route: routes) {
        if (route.get()->match(request)) {
            return route.get()->invokeCallback(request, data);
        }
    }

    // If no route matches return 404 - Not found
    *data = nullptr;
    return RT_Error;
}
