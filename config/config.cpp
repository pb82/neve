#include "config.hpp"

Config::~Config() { }

Config::Config() {
    L = luaL_newstate();
    SET_THIS;
}

Config& Config::i() {
    static Config instance;
    return instance;
}

int Config::config(lua_State *L) {
    GET_THIS(Config, This);

    // Truncate to one argument and make sure its a table
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TSTRING);

    // Get and store the config category
    This->currentCategory = lua_tostring(L, 1);
    This->currentType = ServerConfig;

    lua_remove(L, 1);
    lua_pushcfunction(L, store);

    return 1;
}

int Config::plugin(lua_State *L) {
    GET_THIS(Config, This);

    // Truncate to one argument and make sure its a table
    lua_settop(L, 1);
    luaL_checktype(L, 1, LUA_TSTRING);

    // Get and store the config category
    This->currentCategory = lua_tostring(L, 1);
    This->currentType = PluginConfig;

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

    switch(This->currentType) {
    case ServerConfig:
        This->serverConfig[This->currentCategory] = config;
        break;
    case PluginConfig:
        This->pluginConfig[This->currentCategory] = config;
        break;
    }

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
    lua_register(L, "Plugin", plugin);

    // Run the config script
    if((status = lua_pcall(L, 0, 0, 0)) != LUA_OK) {
        lua_close(L);
        throw ConfigError(lua_tostring(L, -1));
    }

    lua_close(L);
}

std::map<std::string, JSON::Value> &Config::getAll(ConfigType type) {
    return type == ServerConfig
        ? serverConfig
        : pluginConfig;
}

JSON::Value &Config::get(const char *key, ConfigType type) {
    return type == ServerConfig
        ? serverConfig[key]
        : pluginConfig[key];
}

bool Config::has(const char *key, ConfigType type) {
    return type == ServerConfig
        ? serverConfig.find(key) != serverConfig.end()
        : pluginConfig.find(key) != pluginConfig.end();
}

bool Config::has(std::string &key, ConfigType type) {
    return has(key.c_str(), type);
}

JSON::Value& Config::get(std::string &key, ConfigType type) {
    return get(key.c_str(), type);
}
