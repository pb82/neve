#include "logger.hpp"

int Logger::level = Debug;

Logger::Logger() { }
Logger::Logger(int level) {
    Logger::level = level;
}

void Logger::configure(JSON::Value &config) {
    level = config["level"].as<int>();
}

void Logger::log(const char *tag, const char *format, ...) {
    // We don't want multiple threads writing to stdout at the
    // same time
    std::lock_guard<std::mutex> guard(lock);
    va_list argptr;
    va_start(argptr, format);
    std::cout << std::left << std::setw(10) << tag;
    std::vprintf(format, argptr);
    std::cout << std::endl;
    va_end(argptr);
}
