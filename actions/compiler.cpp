#include "compiler.hpp"

bool Compiler::decode() {
	bool result = Base64::Decode(encoded, &decoded);
	if (!result) {
		error = "Decoder error";
		logger.error(error.c_str());
	}

	return result;
}

bool Compiler::compile() {
	int status;
	lua_State *L = luaL_newstate();

	// Check if the chunk was compiled without errors
	if ((status = luaL_loadstring(L, decoded.c_str())) != LUA_OK) {
		error = lua_tostring(L, -1);
		logger.error(error.c_str());
		return false;
	}

	// Write the dumped bytecode to a std::string
	lua_dump(L, writeBytecode, &bytecode, true);
	lua_close(L);
	return true;
}

int Compiler::writeBytecode(lua_State *L, const void *p, size_t sz, void *ud) {
	std::string *buffer = (std::string *) ud;
	buffer->append((const char *) p, sz);
	return 0;
}
