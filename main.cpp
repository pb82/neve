#include <iostream>
#include <string>
#include <cstring>

#include <microhttpd.h>
#include "json/printer.hpp"
#include "json/parser.hpp"
#include "logger/logger.hpp"

#define PORT 8080

int printHeader(void *cls, MHD_ValueKind kind, const char* key, const char *value) {
	std::cout << key << ": " << value << " (" << kind << ")" << std::endl;
	return MHD_YES;
}

int answer(void *cls, MHD_Connection *c, const char *url, const char *method,
           const char *version, const char *upload_data,
           size_t *upload_data_size, void **con_cls) {
	MHD_get_connection_values(c, MHD_HEADER_KIND, &printHeader, nullptr);
	return MHD_NO;
}

int main() {
    MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, nullptr, nullptr, &answer,
                              nullptr, MHD_OPTION_END);

    if (!daemon) {
        exit(1);
    }

    getchar();
    MHD_stop_daemon(daemon);
    return 0;
}
