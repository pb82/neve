#include "loop.hpp"

// Begin static initializations
Logger Loop::logger;
http_parser_settings Loop::settings;
// End static initializations

Loop::Loop(HttpRouter *router) : _router(router) {
	// Http parser callbacks
	settings.on_url = onUrl;
	settings.on_message_complete = onMessageComplete;

	// Http server
	initTcp();
}

Loop::~Loop() {
	uv_stop(uv_default_loop());

	// Close all remaining handles
	uv_walk(uv_default_loop(), [](uv_handle_t *handle, void *){
		uv_close(handle, cleanup);
	}, nullptr);

	// Run once again to invoke dangling callbacks
	run();

	// Try closing until it returns success
	uv_loop_close(uv_default_loop());

	// Auto cleanup the router
	if (_router) delete _router;
}

void Loop::initTcp() {
	uv_tcp_init(uv_default_loop(), &server);
	server.data = _router;

	uv_ip4_addr("0.0.0.0", PORT, &addr);
	uv_tcp_bind(&server, (const sockaddr *) &addr, 0);
	uv_listen((uv_stream_t *) &server, SOMAXCONN, serverOnConnect);
	logger.info("Server listening on port %d", PORT);
}

void Loop::cleanup(uv_handle_t *handle) {
	if (handle->data) delete (HttpRequest *) handle->data;
	if (handle) free(handle);
}

int Loop::onUrl(http_parser *parser, const char *at, size_t length) {
	HttpRequest *request = (HttpRequest *) parser->data;
	request->url = std::string(at, length);
	request->method = parser->method;
	return 0;
}

// Called when parsing a http request is complete
int Loop::onMessageComplete(http_parser *parser) {
	HttpRequest *request = (HttpRequest *) parser->data;
	HttpRouter *router = (HttpRouter *) request->data;

	if (!router) {
		JSON::Value val = "Not found";
		writeResponse(404, request, val);
		return 0;
	}

	// To be initialized by the router callback
	Job *job = nullptr;

	// Match the url against the router
	int code = router->run(request, (void **) &job);

	// Success: status ok
	if (code >= 200 && code < 400) {
		job->code = code;
		job->httpRequest = request;
		uv_queue_work(uv_default_loop(), &job->req, actionRun, actionDone);
	}

	// Error: unknown roue
	else if (code == HTTP_STATUS_NOT_FOUND) {
		logger.warn("Not found: %s", request->url.c_str());

		JSON::Value val = "Not found";
		writeResponse(code, request, val);
	}

	// Error: bad request
	else {
		logger.warn("Bad request: %s", request->url.c_str());

		JSON::Value val = "Bad request";
		writeResponse(code, request, val);
	}

	// Always return 0, otherwise the http parser itself will fail
	return 0;
}

void Loop::serverOnDataIn(uv_stream_t *handle, ssize_t size, const uv_buf_t *buffer) {
	HttpRequest *request = (HttpRequest *) handle->data;

	// Parse incoming data
	if (size > 0) {
		http_parser_execute(&request->parser, &settings, buffer->base, size);
	}

	// EOF received, notify the parser
	else if (size == UV_EOF) {
		http_parser_execute(&request->parser, &settings, buffer->base, 0);
	}

	// Error, close the handle
	else if (size < 0) {
		uv_close((uv_handle_t *) handle, cleanup);
	}

	// Always cleanup the buffer, it's not reused
	if (buffer->base) free(buffer->base);
}

// Called when a new client connects
void Loop::serverOnConnect(uv_stream_t *s, int status) {
	if (status < 0) {
		logger.error("Error on new connection: %s", uv_strerror(status));
		return;
	}

	// Get the client object
	uv_tcp_t *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));
	uv_tcp_init(uv_default_loop(), client);

	// Set up the request object. Will register itself on the client
	new HttpRequest(client, s->data);

	// Accapt the connection
	if (uv_accept(s, (uv_stream_t *) client) == 0) {
		uv_read_start((uv_stream_t *) client,
					  // This lambda allocates the memory for every new collection
					  // It's just allocating the suggested size which is 64kb
					  [](uv_handle_t *handle, size_t size, uv_buf_t *buffer) {
						buffer->base = (char *) malloc(size);
						buffer->len = size;
		}, serverOnDataIn);
	} else {
		uv_close((uv_handle_t *) client, cleanup);
	}
}

void Loop::actionRun(uv_work_t *req) {
	Job *job = static_cast<Job *>(req->data);
	switch(job->jobType) {
	case NOP:
	case PING:
		break;
	default:
		logger.error("Unknown job type %d", job->jobType);
		break;
	}
}

void Loop::actionDone(uv_work_t *req, int status) {
	Job *job = static_cast<Job *>(req->data);
	writeResponse(job->code, job->httpRequest, job->result);

	// Now we can get rid of the job itself (the httprequest will
	// be cleaned up after writing has ended
	delete job;
}

void Loop::writeResponse(int status, HttpRequest *request, JSON::Value& payload) {
	uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));

	HttpResponse response(status, payload);

	logger.info("Response: %s", response.toString().c_str());

	uv_buf_t buf = uv_buf_init((char *) response.toString().c_str(),
							   response.toString().size());

	// Send the response to the client
	uv_write(write_req, (uv_stream_t *) request->client, &buf, 1,
			 // Lambda called to cleanup the resources
			 [](uv_write_t *write_req, int){
				uv_close((uv_handle_t *) write_req->handle, cleanup);
				free(write_req);
	});
}

HttpRouter *const Loop::router() {
	return this->_router;
}

void Loop::run() const {
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
