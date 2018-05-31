#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <iomanip>
#include <cstdarg>
#include <string>
#include <mutex>

#include "../json/value.hpp"

#define LOG_FN(NAME, TAG, MINLEVEL)					\
	void NAME(const char* format) {					\
		if(level < MINLEVEL) return;				\
		log(TAG, format);							\
	}												\
	template<typename... Args>						\
	void NAME(const char *format, Args... args) {   \
		if(level < MINLEVEL) return;				\
		log(TAG, format, args...);					\
	}												\
	template<typename... Args>						\
	void NAME(std::string format, Args... args) {   \
		if(level < MINLEVEL) return;				\
		log(TAG, format.c_str(), args...);			\
	}

enum LogLevel {
	Silent =	1,
	Error =		2,
	Warn =		3,
	Info =		4,
	Debug =		5
};

class Logger
{
public:
	Logger();
	Logger(int level);

	static void configure(JSON::Value &config);

	LOG_FN(error,	"Error",	Error)
	LOG_FN(warn,	"Warn",		Warn)
	LOG_FN(info,	"Info",		Info)
	LOG_FN(debug,	"Debug",	Debug)
private:
	void log(const char *tag, const char *format, ...);
	static int level;
	std::mutex lock;
};

#endif // LOGGER_H
