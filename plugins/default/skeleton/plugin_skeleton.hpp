#ifndef PLUGIN_SKELETON_H
#define PLUGIN_SKELETON_H

#include "../../plugin.hpp"

#define PLUGIN_NAME "skeleton"

class PluginSkeleton : public Plugin {
public:
	PluginSkeleton() { }

	// Interface implementation
	~PluginSkeleton();
	std::string name() const;
	void configure(JSON::Value &config);
	void start();
	JSON::Value call(const std::string &intent, JSON::Value &args);

private:
};

// Creates the entrypoint for the server to create an instance
// of the plugin
EXPORT_PLUGIN(PluginSkeleton)

#endif // PLUGIN_SKELETON_H
