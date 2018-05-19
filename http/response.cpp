#include "response.hpp"

HttpResponse::HttpResponse(int code, JSON::Value &payload) {
	std::string json = printer.print(payload);

	// Construct header
	ss << HTTP_VERSION << statusText(code) << "\r\n";
	ss << "Content-Type: application/json\r\n";
	ss << "Content-Length: " << json.length() << "\r\n";
	ss << "\r\n";
	ss << json;

	response = ss.str();
}

const char* HttpResponse::statusText(int code) const {
	switch(code) {
	case 200:
		return "200 OK";
	case 201:
		return "201 Created";
	case 202:
		return "202 Accepted";
	case 400:
		return "400 Bad Request";
	case 401:
		return "401 Unauthorized";
	case 403:
		return "403 Forbidden";
	case 404:
		return "404 Not Found";
	default:
		return "400 Bad Request";
	}
}

const std::string& HttpResponse::toString() const {
	return response;
}
