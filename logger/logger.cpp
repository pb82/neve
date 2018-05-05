#include "logger.hpp"

Logger::Logger(int level) : level(level) { }

void Logger::log(const char *tag, const char *format, ...) {
	va_list argptr;
	va_start(argptr, format);
	std::cout << std::left << std::setw(10) << tag;
	std::vprintf(format, argptr);
	std::cout << std::endl;
	va_end(argptr);
}
