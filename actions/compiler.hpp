#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <lua.hpp>

#include "../http/base64.hpp"
#include "../logger/logger.hpp"

/**
 * @brief The Compiler class
 * Compiles base64 encoded Lua code into bytecode that can be stored
 * in the database.
 */
class Compiler {
public:
	Compiler(std::string encoded) : encoded(encoded) { }

	/**
	 * @brief decode Decode base64 to source code
	 * Actions are sent to the server encoded as base64. This step decodes
	 * the payload to lua source code.
	 * @return true if the step was successful
	 */
	bool decode();

	/**
	 * @brief compile Compile source code to bytecode
	 * Compile the previously decoded source code to Lua bytecode. This is
	 * done to prevent compilation every time the action is executed
	 * @return
	 */
	bool compile();

	std::string& getBytecode() {
		return bytecode;
	}

	/**
	 * @brief getError error message
	 * Set after either a decoder or a compiler error
	 * @return
	 */
	std::string& getError() {
		return error;
	}

private:
	/**
	 * @brief writeBytecode Write bytecode to string
	 * Executed repeatedly by lua_dump to write chunks of bytecode to
	 * the passed buffer
	 * @param L Lua state
	 * @param p Pointer to the current chunk
	 * @param sz Size of the current chunk
	 * @param ud Pointer to user data
	 * @return Must return 0 to indicate success
	 */
	static int writeBytecode(lua_State *L, const void *p, size_t sz, void *ud);

	std::string encoded;
	std::string decoded;
	std::string bytecode;
	std::string error;

	Logger logger;
};

#endif // COMPILER_H
