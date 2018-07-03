#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <uv.h>

#include "../vendor/http_parser.h"
#include "../json/value.hpp"

/**
 * @brief The HttpRequest struct
 * Handles parsing of a http request message and holds all
 * of the parsed data. This struct manages itself, meaning
 * that if will does all the cleanup required in it's
 * destructor.
 */
struct HttpRequest {
    // No argument constructor for testing
    HttpRequest() : client(nullptr), data(nullptr) { }

    HttpRequest(uv_tcp_t *client, void *data) : client(client), data(data) {
        http_parser_init(&parser, HTTP_REQUEST);
        client->data = this;
        parser.data = this;
    }

    // Every http request needs its own parser state
    http_parser parser;
    uv_tcp_t *client;
    void *data;

    // Path params
    JSON::Object params;

    // Parsed data
    int method;
    std::string url;
    std::string body;
};

#endif // REQUEST_H
