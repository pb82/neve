#include "registry.hpp"

PluginRegistry::PluginRegistry() { }

PluginRegistry::~PluginRegistry() {

}

PluginRegistry& PluginRegistry::i() {
	static PluginRegistry instance;
	return instance;
}
