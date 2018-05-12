#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <uv.h>

#include "../http-parser/http_parser.h"

/**
 * @brief The HttpRequest struct
 * Handles parsing of a http request message and holds all
 * of the parsed data. This struct manages itself, meaning
 * that if will does all the cleanup required in it's
 * destructor.
 */
struct HttpRequest {
	HttpRequest(uv_tcp_t *client) : client(client) {
		http_parser_init(&parser, HTTP_REQUEST);
		client->data = this;
		parser.data = this;
	}

	// Every http request needs it's own parser state
	http_parser parser;
	uv_tcp_t *client;

	// Begin parsed data
	std::string url;
	// End parsed data
};

#endif // REQUEST_H
