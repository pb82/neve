#ifndef SANDBOX_H
#define SANDBOX_H

#include <chrono>
#include <csetjmp>
#include <lua.hpp>

#include "action.hpp"
#include "../macros.hpp"
#include "../json/value.hpp"

#define LUA_GLOBAL "_G"
#define GRANULARITY 100000

#define UPDATE_MS(Self,Target)                                              \
	Self->Target = std::chrono::duration_cast<std::chrono::milliseconds>(   \
		std::chrono::system_clock::now().time_since_epoch()                 \
	).count() - Self->started

enum RunCode {
	Success = 1,
	ErrTimeout,
	ErrBytecode,
	ErrPriming,
	ErrNoMain,
	ErrMain,
	ErrMemory,
	ErrUnknown
};

class Sandbox {
public:
	Sandbox(Action *action);
	~Sandbox();

	RunCode run(JSON::Value &args, JSON::Value &result, std::string *msg);

private:
	void loadLibraries();

	static void hook(lua_State *L, lua_Debug *);

	// The action to run
	Action *action;

	/**
	 * @brief running
	 * Used to track the time a script is running (in milliseconds). This
	 * us updated after a certain number of instructions. Public
	 * because this needs to be accesses in the static `hook`
	 * handler.
	 */
	long running = 0;
	long started = 0;

	lua_State *L;
	jmp_buf env;
};

#endif // SANDBOX_H
