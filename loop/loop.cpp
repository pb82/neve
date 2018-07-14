#include "loop.hpp"

// Begin static initializations
Logger Loop::logger;
http_parser_settings Loop::settings;
// End static initializations

Loop::Loop(JSON::Value config, HttpRouter *router) : config(config), _router(router) {
    // Http parser callbacks
    settings.on_url = onUrl;
    settings.on_body = onBody;
    settings.on_message_complete = onMessageComplete;
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

void Loop::parse() {
    if (!config.is(JSON::JSON_OBJECT)) {
        throw ConfigError("Invalid config, must be an object");
    }

    // Get port from config
    if (!config["port"].is(JSON::JSON_NUMBER)) {
        throw ConfigError("Invalid config, `port' must be a number");
    }
    port = config["port"].as<int>();

    // Get IP address from config
    if (!config["ipaddress"].is(JSON::JSON_STRING)) {
        throw ConfigError("Invalid config, `ipaddress' must be a string");
    }
    ipaddress = config["ipaddress"].as<std::string>();
}

void Loop::initTcp() {
    parse();

    int status = uv_tcp_init(uv_default_loop(), &server);
    if (status) {
        logger.error("Error in uv_tcp_init: %d", status);
        return;
    }

    server.data = _router;

    uv_ip4_addr(ipaddress.c_str(), port, &addr);
    status = uv_tcp_bind(&server, (const sockaddr *) &addr, 0);
    if (status) {
        logger.error("Error in uv_tcp_bind: %d", status);
        return;
    }

    status = uv_listen((uv_stream_t *) &server, SOMAXCONN, serverOnConnect);
    if (status) {
        logger.error("Error in uv_listen: %d", status);
        return;
    }

    logger.info("Server listening on %s:%d", ipaddress.c_str(), port);
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

// Read POST request body
int Loop::onBody(http_parser *parser, const char *at, size_t length) {
    HttpRequest *request = (HttpRequest *) parser->data;

    // This method could be called multiple times for a single request so
    // we need to use append
    request->body.append(std::string(at, length));
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
    RunType runType = router->run(request, (void **) &job);
    if (runType == RT_Sync) {
        job->setBlock(true);
        job->setHttpRequest(request);
        uv_queue_work(uv_default_loop(), job->getWorkRequest(), actionRun, actionDone);
    }

    else if (runType == RT_Async) {
        // Generate a UUID and return it immediately. The UUID will be used
        // to identify the result of the job when it is requested.
        uint uuid = UUID::create();
        logger.debug("Launching async job with uuid %d", uuid);

        job->setBlock(false);
        job->setUUID(uuid);

        // Immediately return the UUID to the caller
        JSON::Value val = 195111;
        // uv_queue_work(uv_default_loop(), job->getWorkRequest(), actionRun, actionDone);
        writeResponse(200, request, val);
    }

    else if (runType == RT_Delayed) {
        uint token = request->params["token"].as<uint>();
        Reactor::i().placeCallback(token, [](Job *job) {
            writeResponse(job->getCode(), job->getHttpRequest(), job->getResult());
        });
    }

    // Router returned false, so either the route was not found or the callback
    // retured false. Either way we respond with 404
    else {
        JSON::Value val = "Not found";
        writeResponse(404, request, val);
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

    // Execute will set the 'code' and 'result' properties of
    // the job
    job->execute();
}

void Loop::actionDone(uv_work_t *req, int) {
    Job *job = static_cast<Job *>(req->data);

    // Only write the response if the client was waiting (used the
    // --block flag)
    if (job->getBlock()) {
        writeResponse(job->getCode(), job->getHttpRequest(), job->getResult());
    } else {
        Reactor::i().placeResult(job);
    }

    // Now we can get rid of the job itself (the httprequest will
    // be cleaned up after writing has ended
    delete job;
}

void Loop::writeResponse(int status, HttpRequest *request, JSON::Value& payload) {
    uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));

    HttpResponse response(status, payload);

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
