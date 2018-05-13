#include "config.hpp"

Config::Config() {
	L = luaL_newstate();
	SET_THIS;
}

Config::~Config() {
	lua_close(L);
}

int Config::config(lua_State *L) {
	GET_THIS(Config, This);

	// Truncate to one argument and make sure its a table
	lua_settop(L, 1);
	luaL_checktype(L, 1, LUA_TSTRING);

	// Get and store the config category
	This->currentCategory = lua_tostring(L, 1);

	lua_remove(L, 1);
	lua_pushcfunction(L, store);

	return 1;
}

int Config::store(lua_State *L) {
	GET_THIS(Config, This);

	// Truncate to one argument and make sure its a table
	lua_settop(L, 1);
	luaL_checktype(L, 1, LUA_TTABLE);

	JSON::Value config;
	config.fromLua(L);

	This->serverConfig[This->currentCategory] = config;

	return 0;
}

void Config::load(const char *file) {
	int status = 0;

	// Load the config script
	if ((status = luaL_loadfile(L, file)) != LUA_OK) {
		throw ConfigError(lua_tostring(L, -1));
	}

	// Register config callbacks
	lua_register(L, "Config", config);

	// Run the config script
	if((status = lua_pcall(L, 0, 0, 0)) != LUA_OK) {
		throw ConfigError(lua_tostring(L, -1));
	}
}

JSON::Value &Config::get(const char *key) {
	return serverConfig[key];
}
