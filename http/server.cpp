#include "server.hpp"

JSON::Printer HttpServer::printer;

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
			path.get()->callback([c](JSON::Value *result){
				MHD_resume_connection(c);

				std::string json = printer.print(*result);

				MHD_Response *response = nullptr;
				response = MHD_create_response_from_buffer(json.size(),
					(void *) json.c_str(), MHD_RESPMEM_MUST_COPY);

				MHD_add_response_header(response, "Content-Type", "text/plain");
				MHD_queue_response(c, MHD_HTTP_OK, response);
				MHD_destroy_response(response);
			});
			MHD_suspend_connection(c);
			return MHD_YES;
		}
	}

	// No suitable route found
	const char *message = "Not found";
	MHD_Response *response = nullptr;
	response = MHD_create_response_from_buffer(std::strlen(message),
		(void *) message, MHD_RESPMEM_PERSISTENT);

	int ret = MHD_queue_response(c, MHD_HTTP_NOT_FOUND, response);
	MHD_destroy_response(response);
	return ret;
}

bool HttpServer::run() {
	logger.info("starting server on port %d", port);

	server = MHD_start_daemon(				
				MHD_USE_SELECT_INTERNALLY | MHD_USE_SUSPEND_RESUME,
				port,			// Port
				nullptr,		// Accept policy callback
				nullptr,		// Accept policy data
				&onRequest,		// Request handler
				this,			// Request handler data
				MHD_OPTION_END);

	return server != nullptr;
}
