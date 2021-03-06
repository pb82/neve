#ifndef ROUTER_H
#define ROUTER_H

#include <map>
#include <vector>
#include <memory>

#include "path.hpp"
#include "request.hpp"

class HttpRouter {
public:
    ~HttpRouter();

    /**
     * @brief get Register a GET request handler
     * @param route The path pattern to register the handler for
     * @param cb The callback invoked when the pattern matches the
     * current path
     */
    void get(const char *route, MatchCallback cb);

    /**
     * @brief get Register a POST request handler
     * @param route The path pattern to register the handler for
     * @param cb The callback invoked when the pattern matches the
     * current path
     */
    void post(const char *route, MatchCallback cb);

    /**
     * @brief del Register a DELETE request handler
     * @param route The path pattern to register the handler for
     * @param cb The callback invoked when the pattern matches the
     * current path
     */
    void del(const char *route, MatchCallback cb);

    /**
     * @brief put Register a PUT request handler
     * @param route The path pattern to register the handler for
     * @param cb The callback invoked when the pattern matches the
     * current path
     */
    void put(const char *route, MatchCallback cb);

    /**
     * @brief run Run the router and try to match the given path agains
     * all patterns
     * @param method Http method
     * @param path URL
     * @param data A pointer to a pointer that can be initialized with a
     * Job to be run on the queue
     * @return true if a matching route was found and run
     */
    RunType run(HttpRequest *request, void **data) const;
private:
    void registerHandler(int method, const char *route, MatchCallback cb);
    std::vector<std::unique_ptr<Path>> routes;
};

#endif // ROUTER_H

