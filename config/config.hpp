#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>

#include "../macros.hpp"
#include "../json/value.hpp"

/**
 * @brief The ConfigError class
 * Thrown when the config file is not found or when an error
 * occurs in the configuration script
 */
class ConfigError : public std::runtime_error
{
public:
	ConfigError(std::string message)
		: std::runtime_error(message)
	{
	}
};

/**
 * @brief The Config class
 * Loads and stores configuration categories from config.lua
 * and makes it accessible to the server and it's plugins. Meant
 * to be the central point of configuration.
 */
class Config {
public:
	Config();
	~Config();

	// Load configuration from file
	void load(const char *file);

	// Get configuration for the given category
	JSON::Value &get(const char *key);

private:
	// Lua callbacks
	static int config(lua_State *L);
	static int store(lua_State *L);

	lua_State *L;
	const char* currentCategory;

	// Stores the actual configuration categories
	std::map<std::string, JSON::Value> serverConfig;
};

#endif // CONFIG_H
