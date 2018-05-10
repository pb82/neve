#include "loop.hpp"

// Begin static initializations
Logger Loop::logger;
// End static initializations

Loop::Loop() {
	initTcp();
}

Loop::~Loop() {
	uv_stop(uv_default_loop());

	// Close all remaining handles
	uv_walk(uv_default_loop(), close, nullptr);
	run();

	// Try closing until it returns success
	uv_loop_close(uv_default_loop());
}

void Loop::initTcp() {
	uv_tcp_init(uv_default_loop(), &server);
	uv_ip4_addr("0.0.0.0", PORT, &addr);
	uv_tcp_bind(&server, (const sockaddr *) &addr, 0);
	uv_listen((uv_stream_t *) &server, SOMAXCONN, socketConnect);

	logger.info("Server listening on port %d", PORT);
}

void Loop::close(uv_handle_t *handle, void *) {
	uv_close(handle, cleanup);
}

void Loop::cleanup(uv_handle_t *handle) {
	if (handle) free(handle);
}

void Loop::allocateMemory(uv_handle_t *handle, size_t size, uv_buf_t *buffer) {
	buffer->base = (char *) malloc(size);
	buffer->len = size;
}

void Loop::writeResponse(uv_write_t *write_req, int status) {
	uv_close((uv_handle_t *) write_req->handle, cleanup);
	free(write_req);
}

void Loop::readClientData(uv_stream_t *handle, ssize_t size, const uv_buf_t *buffer) {
	// Read error
	if (size < 0) {
		if (buffer->base) free(buffer->base);
	}

	// No data but still no EOF
	// Don nothing
	else if (size == 0) {
		return;
	}

	else if (size > 0) {

	}

	// Done reading
	else if (size == UV__EOF) {
		free(buffer->base);
		uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
		uv_buf_t buf = uv_buf_init((char *) RESPONSE, std::strlen(RESPONSE));
		uv_write(req, handle, &buf, 1, writeResponse);
	}
}

void Loop::socketConnect(uv_stream_t *s, int status) {
	if (status < 0) {
		logger.error("Error on new connection: %s", uv_strerror(status));
		return;
	}

	uv_tcp_t *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));
	uv_tcp_init(uv_default_loop(), client);	

	if (uv_accept(s, (uv_stream_t *) client) == 0) {
		uv_read_start((uv_stream_t *) client, allocateMemory, readClientData);
	} else {
		uv_close((uv_handle_t *) client, cleanup);
	}
}

void Loop::run() const {
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
