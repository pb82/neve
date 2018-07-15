#include "registry.hpp"

PluginRegistry::PluginRegistry() { }

PluginRegistry::~PluginRegistry() {
    cleanup();
}

PluginRegistry& PluginRegistry::i() {
    static PluginRegistry instance;
    return instance;
}

void PluginRegistry::cleanup() {
    // Iterate over all handles and invoke the destructor
    // for every instance
    for (const auto& pair : handles) {
        PluginHandle *handle = pair.second.get();
        for (Plugin *instance : handle->instances) {
            if (!handle->cleanup) {
                instance->globalCleanup();
                handle->cleanup = true;
            }

            handle->destructor(instance);

            // Close the library and free the memory of the
            // handle
            uv_dlclose(handle->handle);
            free(handle->handle);
        }
    }
}

Plugin *PluginRegistry::newInstance(std::string name, std::string& path,
                                    JSON::Value &config, bool registered) {
    logger.debug("Creating a new instance of plugin %s", name.c_str());
    PluginHandle* handle = getHandle(name, path);
    if (!handle) {
        return nullptr;
    }

    Plugin *instance = handle->constructor();

    try {
        // Make sure global init is only performed once
        if (!handle->initialized) {
            instance->globalInit();
            handle->initialized = true;
        }

        instance->configure(config);
        instance->start();
    } catch (std::runtime_error err) {
        logger.error("Error starting plugin %s: %s", name.c_str(), err.what());
        handle->destructor(instance);
        return nullptr;
    }

    if (registered) handle->instances.push_back(instance);
    return instance;
}

void PluginRegistry::destroyUnregisteredInstance(std::string name, std::string path,
                                                 Plugin *instance) {
    PluginHandle *handle = getHandle(name, path);
    handle->destructor(instance);
}

PluginHandle* PluginRegistry::getHandle(std::string name, std::string& path) {
    if (handles.find(name) != handles.end()) {
        logger.debug("Using cached handle for plugin %s", name.c_str());
        return handles[name].get();
    }

    PluginHandle *handle = createHandle(path);
    if (!handle) {
        return nullptr;
    }

    handles[name] = std::unique_ptr<PluginHandle>(handle);
    return handle;
}

PluginHandle *PluginRegistry::createHandle(std::string &path) {
    logger.debug("Creating a new handle for library %s", path.c_str());

    int status = 0;
    uv_lib_t *lib = (uv_lib_t *) malloc(sizeof(uv_lib_t));
    status = uv_dlopen(path.c_str(), lib);

    if (status != 0) {
        logger.error("Error loading library %s: (%s)", path.c_str(), uv_dlerror(lib));
        free(lib);
        return nullptr;
    }

    PluginHandle *handle = new PluginHandle;
    handle->handle = lib;

    status = uv_dlsym(lib, PLUGIN_CREATE,	(void **) &handle->constructor);
    status = uv_dlsym(lib, PLUGIN_DESTROY,	(void **) &handle->destructor);

    if (status != 0) {
        logger.error("Error loading symbols from %s: (%s)",
            path.c_str(), uv_dlerror(lib));
        uv_dlclose(lib);
        return nullptr;
    }

    logger.debug("Successfully created handle for %s", path.c_str());
    return handle;
}
