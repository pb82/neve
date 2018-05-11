#ifndef LOOP_H
#define LOOP_H

#include <uv.h>
#include <cstring>
#include <thread>

#include "../logger/logger.hpp"
#include "../http-parser/http_parser.h"

#define PORT 8080

#define RESPONSE					\
	"HTTP/1.1 200 OK\r\n"           \
	"Content-Type: text/plain\r\n"  \
	"Content-Length: 14\r\n"        \
	"\r\n"                          \
	"Hello, World!\n"

enum JobType {
	NOP =	1,
	PING =	2
};

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
	~HttpRequest() {
		if (url) free(url);
	}

	// Every http request needs it's own parser state
	http_parser parser;
	uv_tcp_t *client;

	// Begin parsed data
	char *url;
	// End parsed data
};

struct Job {
	HttpRequest *httpRequest;
	JobType jobType;
	uv_work_t req;
};

class Loop {
public:
	Loop();
	~Loop();

	/**
	 * @brief run Start the event loop. This method
	 * will block
	 */
	void run() const;
private:
	void initTcp();

	// libuv callbacks
	static void serverOnConnect(uv_stream_t *server, int status);
	static void serverOnDataIn(uv_stream_t *handle, ssize_t size, const uv_buf_t *buffer);
	static void serverAfterWrite(uv_write_t* write_req, int);
	static void cleanup(uv_handle_t *handle);

	// http-parser callbacks
	static int messageComplete(http_parser *parser);
	static int onUrl(http_parser *parser, const char *at, size_t length);

	// work queue callbacks
	static void actionRun(uv_work_t *req);
	static void actionDone(uv_work_t *req, int status);

	uv_tcp_t server;
	sockaddr_in addr;
	static Logger logger;
	static http_parser_settings settings;
};

#endif // LOOP_H

