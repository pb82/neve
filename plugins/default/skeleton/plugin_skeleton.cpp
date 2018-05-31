#include "plugin_skeleton.hpp"

std::string PluginSkeleton::name() const {
	return PLUGIN_NAME;
}

PluginSkeleton::~PluginSkeleton() {
}

void PluginSkeleton::start() {
}

void PluginSkeleton::configure(JSON::Value &config) {
}

JSON::Value PluginSkeleton::call(const std::string &intent, JSON::Value &args) {
	return JSON::null;
}
