#ifndef LOOP_H
#define LOOP_H

#include <uv.h>
#include <cstring>

#include "../logger/logger.hpp"

#define PORT 8080

#define RESPONSE                  \
  "HTTP/1.1 200 OK\r\n"           \
  "Content-Type: text/plain\r\n"  \
  "Content-Length: 14\r\n"        \
  "\r\n"                          \
  "Hello, World!\n"

class Loop {
public:
	Loop();
	~Loop();

	void run() const;
private:
	void initTcp();

	static void socketConnect(uv_stream_t *server, int status);
	static void allocateMemory(uv_handle_t *handle, size_t size, uv_buf_t *buffer);
	static void readClientData(uv_stream_t *handle, ssize_t size, const uv_buf_t *buffer);
	static void writeResponse(uv_write_t* write_req, int status);
	static void close(uv_handle_t *handle, void *);
	static void cleanup(uv_handle_t *handle);

	uv_tcp_t server;
	sockaddr_in addr;
	static Logger logger;
};

#endif // LOOP_H

