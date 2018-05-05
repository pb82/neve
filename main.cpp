#include <iostream>
#include <string>
#include <cstring>

#include <microhttpd.h>
#include "json/printer.hpp"
#include "json/parser.hpp"

#define PORT 8080

int answer(void *cls, MHD_Connection *c, const char *url, const char *method,
           const char *version, const char *upload_data,
           size_t *upload_data_size, void **con_cls) {
    int ret;
    static int aptr;

    if (&aptr != *con_cls) {
        *con_cls = &aptr;        
        return MHD_YES;
    }


	JSON::Printer printer;
	JSON::Parser parser;
	JSON::Value val;

	parser.parse(val, "{\"x\":1{");

	std::string page = printer.print(val);

    MHD_Response *res;
    res = MHD_create_response_from_buffer(page.size(), (void *) page.c_str(),
                                          MHD_RESPMEM_MUST_COPY);

    std::cout << "URL: " << url << std::endl;

    ret = MHD_queue_response(c, MHD_HTTP_OK, res);
    MHD_destroy_response(res);
    return ret;
}

int main() {
    MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, nullptr, nullptr, &answer,
                              nullptr, MHD_OPTION_END);

    if (!daemon) {
        exit(1);
    }

    std::cout << "Server running on port " << PORT << std::endl;

    getchar();
    MHD_stop_daemon(daemon);
    return 0;
}
