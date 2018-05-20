#ifndef RESPONSE_H
#define RESPONSE_H

#include <sstream>

#include "../json/value.hpp"
#include "../json/printer.hpp"

#define HTTP_VERSION "HTTP/1.1 "

class HttpResponse {
public:
	HttpResponse(int code, JSON::Value &payload);
	std::string& toString();

private:
	const char* statusText(int code) const;

	std::string response;
	std::stringstream ss;
	JSON::Printer printer;
};

#endif // RESPONSE_H
