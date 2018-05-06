#include "server.hpp"

HttpServer::HttpServer(int port) : port(port) { }

HttpServer::~HttpServer() {
	if (server) {
		MHD_stop_daemon(server);
	}
}

void HttpServer::get(const char *route, MatchCallback callback) {
	Path *path = new Path("GET", route);
	path->callback = callback;
	routes.push_back(std::unique_ptr<Path>(path));
}

MHD_Response *HttpServer::replyText(const char *message) {
	MHD_Response *response = nullptr;
	response = MHD_create_response_from_buffer(std::strlen(message), (void *) message,
											   MHD_RESPMEM_PERSISTENT);
	MHD_add_response_header(response, "Content-Type", "text/plain");
	return response;
}

int HttpServer::onRequest(void *cls, MHD_Connection *c, const char *url,
						  const char *method, const char *, const char *, size_t *,
						  void **ptr) {
	static int marker;
	HttpServer *This = static_cast<HttpServer *>(cls);

	// Don't respond on first call but set the marker
	if (&marker != *ptr) {
		*ptr = &marker;
		return MHD_YES;
	}

	// Reset marker to indicate we're done
	*ptr = nullptr;

	for (auto& path: This->routes) {
		if (path.get()->match(method, url)) {
			path.get()->callback([c](){
				MHD_Response *response = replyText("OK");
				MHD_queue_response(c, MHD_HTTP_OK, response);
				MHD_destroy_response(response);
			});
			return MHD_YES;
		}
	}

	// No suitable route found
	MHD_Response *response = replyText("Not found");
	int ret = MHD_queue_response(c, MHD_HTTP_NOT_FOUND, response);
	MHD_destroy_response(response);
	return ret;
}

bool HttpServer::run() {
	logger.info("starting server on port %d", port);

	server = MHD_start_daemon(
				MHD_USE_SELECT_INTERNALLY,
				port,			// Port
				nullptr,		// Accept policy callback
				nullptr,		// Accept policy data
				&onRequest,		// Request handler
				this,			// Request handler data
				MHD_OPTION_END);

	return server != nullptr;
}
