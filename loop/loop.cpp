#include "loop.hpp"

// Begin static initializations
Logger Loop::logger;
http_parser_settings Loop::settings;
// End static initializations

Loop::Loop() {
	settings.on_url = onUrl;
	settings.on_message_complete = messageComplete;
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
}

void Loop::initTcp() {
	uv_tcp_init(uv_default_loop(), &server);
	uv_ip4_addr("0.0.0.0", PORT, &addr);
	uv_tcp_bind(&server, (const sockaddr *) &addr, 0);
	uv_listen((uv_stream_t *) &server, SOMAXCONN, serverOnConnect);
	logger.info("Server listening on port %d", PORT);
}

void Loop::cleanup(uv_handle_t *handle) {
	if (handle->data) delete (HttpRequest *) handle->data;
	if (handle) free(handle);
}

void Loop::serverAfterWrite(uv_write_t *write_req, int) {
	uv_close((uv_handle_t *) write_req->handle, cleanup);
	free(write_req);
}

int Loop::onUrl(http_parser *parser, const char *at, size_t length) {
	HttpRequest *request = (HttpRequest *) parser->data;
	request->url = (char *) malloc(length);
	std::memcpy(request->url, at, length);
	return 0;
}

// Called when parsing a http request is complete
int Loop::messageComplete(http_parser *parser) {
	HttpRequest *request = (HttpRequest *) parser->data;

	// Send response
	uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));
	uv_buf_t buf = uv_buf_init((char *) RESPONSE, sizeof(RESPONSE));
	uv_write(write_req, (uv_stream_t *) request->client, &buf, 1, serverAfterWrite);

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
	new HttpRequest(client);

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

}

void Loop::actionDone(uv_work_t *req, int status) {

}

void Loop::run() const {
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
