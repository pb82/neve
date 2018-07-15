#ifndef REGISTRY_H
#define REGISTRY_H

#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include <uv.h>

#include "../config/config.hpp"
#include "../logger/logger.hpp"
#include "../json/value.hpp"
#include "plugin.hpp"

typedef Plugin *(*PluginNew)();
typedef void (*PluginDestroy)(Plugin *i);

/**
 * @brief The PluginHandle struct
 * Manages creation and destruction of a single Plugin class
 * and keeps tracks of it's instances
 */
struct PluginHandle {
    // Constructor and destructor are read from the library
    // itself using dlsym
    PluginNew constructor;
    PluginDestroy destructor;
    uv_lib_t *handle;

    // List of all the instances that were created using the
    // constructor function
    std::vector<Plugin *> instances;

    // Keeps track on whether the global initialization and
    // clean up has been perormed for this handle
    bool initialized = false;
    bool cleanup = false;
};

/**
 * @brief The PluginRegistry class
 * Manages plugin instances and allows creating and
 * calling them.
 */
class PluginRegistry {
public:
    static PluginRegistry& i();
    ~PluginRegistry();

    PluginRegistry(PluginRegistry const&) = delete;
    void operator=(PluginRegistry const&) = delete;

    void loadFromConfig();

    /**
     * @brief newInstance Create new plugin instance
     * Create a new instance of the plugin. This does not register the plugin,
     * it only returns a pointer to the instance.
     * @param path The path to the plugin which is expected to be in the form
     * of a shared library
     * @return A pointer to the plugin instance
     */
    Plugin *newInstance(std::string name, std::string& path, JSON::Value& config);
    Plugin *getInstance(std::string name);

private:
    PluginHandle *getHandle(std::string name, std::string &path);
    PluginHandle* createHandle(std::string &path);
    void cleanup();

    std::map<std::string, std::unique_ptr<PluginHandle>> handles;
    std::map<std::string, Plugin *> instances;

    PluginRegistry();
    Logger logger;
    std::mutex mutex;
};

#endif // REGISTRY_H
