#ifndef RUNTIME_CONFIG_H
#define RUNTIME_CONFIG_H

#include <string>
#include <map>
#include <yaml-cpp/yaml.h>

#include "mudconfig.h"

/// stores config data
/** This class manages runtime configuration values that may be changed
	while the server is running.
	\note There are 4 types of config data: strings, ints, floats, and bools. Each
		is saved in its own map and has its own section in the config.yaml file.
*/
class RuntimeConfig {
public:
	RuntimeConfig();
	~RuntimeConfig();

	void save() const;

	void set(const std::string &key, const std::string &value);
	void set(const std::string &key, const int value);
	void set(const std::string &key, const float value);
	void set(const std::string &key, bool value);

	std::string getStringValue(const std::string &key) const;
	int getIntValue(const std::string &key) const;
	float getFloatValue(const std::string &key) const;
	bool getBoolValue(const std::string &key) const;

private:
	IOResourceLocator mResLoc;

	StringMap mConfigStrings;
	std::map<std::string, int> mConfigInts;
	std::map<std::string, float> mConfigFloats;
	std::map<std::string, bool> mConfigBools;

	bool load();
	bool loadStrings(const YAML::Node &node);
	bool loadInts(const YAML::Node &node);
	bool loadFloats(const YAML::Node &node);
	bool loadBools(const YAML::Node &node);

};

#endif // RUNTIME_CONFIG_H
