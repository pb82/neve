#ifndef LOOP_H
#define LOOP_H

#include <uv.h>
#include <cstring>
#include <thread>

#include "../logger/logger.hpp"
#include "../json/value.hpp"
#include "../vendor/http_parser.h"
#include "../http/router.hpp"
#include "../http/request.hpp"
#include "../http/response.hpp"
#include "../jobs/job.hpp"
#include "../config/config.hpp"

class Loop {
public:
	Loop(JSON::Value config, HttpRouter *router);
	~Loop();

	/**
	 * @brief run Start the event loop. This method
	 * will block
	 */
	void run() const;
	void initTcp();

	HttpRouter *const router();
private:
	void parse();

	// libuv callbacks
	static void serverOnConnect(uv_stream_t *server, int status);
	static void serverOnDataIn(uv_stream_t *handle, ssize_t size, const uv_buf_t *buffer);
	static void cleanup(uv_handle_t *handle);

	// http-parser callbacks
	static int onMessageComplete(http_parser *parser);
	static int onUrl(http_parser *parser, const char *at, size_t length);
	static int onBody(http_parser *parser, const char *at, size_t length);

	// work queue callbacks
	static void actionRun(uv_work_t *req);
	static void actionDone(uv_work_t *req, int status);

	// Constructs and sends a Http response
	static void writeResponse(int status, HttpRequest *request, JSON::Value &payload);

	static Logger logger;
	static http_parser_settings settings;

	// Parsed properties
	JSON::Value config;
	std::string ipaddress;
	int port;
	
	uv_tcp_t server;
	sockaddr_in addr;
	HttpRouter *_router;
};

#endif // LOOP_H

