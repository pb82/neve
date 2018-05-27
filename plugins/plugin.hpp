#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>

#include "../json/value.hpp"

/**
 * @brief The Plugin class
 * Base class for all plugins. Provides methods to instantiate, configure,
 * run and cleanup a plugin.
 */
class Plugin
{
public:
	/**
	 * @brief ~Plugin destructor
	 * Plugins should implement a destructor to cleanup all
	 * resources (db connections, memory, files) that they
	 * allocated during their runtime.
	 */
	virtual ~Plugin() = 0;

	/**
	 * @brief name Plugin name
	 * Should return the name of the plugin. This is the name
	 * that actions will use to call it.
	 * @return string The name of the plugin
	 */
	virtual std::string name() = 0;

	/**
	 * @brief configure Configure plugin
	 * Passes the configuration object defined in config.lua down
	 * to the plugin. The name of the plugin is used to identify
	 * the configuration
	 * @param config JSON::Value usually an object with user defined
	 * config properties
	 */
	virtual void configure(JSON::Value &config) = 0;

	/**
	 * @brief start Start the plugin
	 * The start function will be called after configure and is intended
	 * to set up all resources required to run the plugin (db connections,
	 * files, etc.)
	 */
	virtual void start() = 0;

	/**
	 * @brief call Call plugin
	 * @param intent Can be used to implement different features of a plugin, e.g.
	 * the mongo plugin will use this to differentiation between CRUDLA actions
	 * @param args arguments passed to the plugin
	 * @return JSON::Value call result
	 */
	virtual JSON::Value call(const std::string& intent, JSON::Value& args) = 0;
};

#endif // PLUGIN_H
