#ifndef SERVER_H
#define SERVER_H

#include <microhttpd.h>
#include <functional>
#include <cstring>
#include <vector>
#include <memory>

#include "../logger/logger.hpp"
#include "../json/printer.hpp"
#include "path.hpp"

typedef std::function<void(JSON::Value *result)> DoneCallback;

class HttpServer {
public:
	HttpServer(int port);
	~HttpServer();

	bool run();
	void get(const char *route, MatchCallback callback);
private:
	static MHD_Response *replyText(const char *message);

	static int onRequest(void *cls, MHD_Connection *c, const char *url, const char *method,
						 const char *, const char *, size_t *, void **ptr);

	MHD_Daemon *server = nullptr;
	int port;

	Logger logger;
	static JSON::Printer printer;
	std::vector<std::unique_ptr<Path>> routes;
};

#endif // SERVER_H
