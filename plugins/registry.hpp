#ifndef REGISTRY_H
#define REGISTRY_H

class PluginRegistry {
public:
	static PluginRegistry& i();
	~PluginRegistry();

	PluginRegistry(PluginRegistry const&) = delete;
	void operator=(PluginRegistry const&) = delete;

private:
	PluginRegistry();
};

#endif // REGISTRY_H
