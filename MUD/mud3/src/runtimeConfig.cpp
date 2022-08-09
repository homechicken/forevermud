#include "runtimeConfig.h"

#include "global.h"
extern Global glob;

/// constructor
/** The constructor sets the location of the config file and calls the load()
	function.
*/
RuntimeConfig::RuntimeConfig() {
	mResLoc.type = DataObject;
	mResLoc.name = "config.yaml";
	load();
}

/// destructor
/** The destructor saves the config data before it exits.
*/
RuntimeConfig::~RuntimeConfig() {
	save();
}

/// saves the config
/** This function saves the current configuration data to storage.
*/
void RuntimeConfig::save() const {
	YAML::Emitter out;

	out << YAML::BeginMap;

	out << YAML::Key << "Strings" << YAML::Value << YAML::BeginMap;

	if(mConfigStrings.size() > 0) {
		for(StringMap::const_iterator it = mConfigStrings.begin(); it != mConfigStrings.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
		}
	} else {
		out << YAML::Null;
	}
	out << YAML::EndMap;

	out << YAML::Key << "Integers" << YAML::Value << YAML::BeginMap;

	if(mConfigInts.size() > 0) {
		for(std::map<std::string, int>::const_iterator it = mConfigInts.begin(); it != mConfigInts.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
		}
	} else {
		out << YAML::Null;
	}
	out << YAML::EndMap;

	out << YAML::Key << "Floats" << YAML::Value << YAML::BeginMap;

	if(mConfigFloats.size() > 0) {
		for(std::map<std::string, float>::const_iterator it = mConfigFloats.begin(); it != mConfigFloats.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
		}
	} else {
		out << YAML::Null;
	}
	out << YAML::EndMap;

	out << YAML::Key << "Booleans" << YAML::Value << YAML::BeginMap;

	if(mConfigBools.size() > 0) {
		for(std::map<std::string, bool>::const_iterator it = mConfigBools.begin(); it != mConfigBools.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
		}
	} else {
		out << YAML::Null;
	}
	out << YAML::EndMap;

	out << YAML::EndMap;

	if(!out.good()) {
		glob.log.error(boost::format("RuntimeConfig::save(): YAML Emitter is in a bad state: %1%") % out.GetLastError());
	} else {
		if(!glob.ioDaemon.saveResource(mResLoc, out.c_str())) {
			glob.log.error("RuntimeConfig::save: failed to save the ban map");
		}
	}
}

/// loads the config data
/** This function loads the configuration data.
	\return true if the data loaded properly
*/
bool RuntimeConfig::load() {
	glob.log.info("Loading MUD configuration");
	std::string configData = glob.ioDaemon.getResource(mResLoc);

	if(configData == IO_RESOURCE_NOT_FOUND) {
		glob.log.warn("RuntimeConfig::load(): No configuration file found");
		return false;
	}

	std::istringstream is(configData);

	bool success = false;

	try {
		YAML::Parser parser(is);
		YAML::Node doc;
		parser.GetNextDocument(doc);

		std::string greeting;
		doc["Greeting"] >> greeting;
		mConfigStrings.insert(make_pair("Greeting", greeting));

		if(!loadStrings(doc["Strings"])) {
			glob.log.error("RuntimeConfig::load: Strings cannot be loaded");
		} else if(!loadInts(doc["Integers"])) {
			glob.log.error("RuntimeConfig::load: Integers cannot be loaded");
		} else if(!loadFloats(doc["Floats"])) {
			glob.log.error("RuntimeConfig::load: Floats cannot be loaded");
		} else if(!loadBools(doc["Booleans"])) {
			glob.log.error("RuntimeConfig::load: Booleans cannot be loaded");
		} else {
			success = true;
		}
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("RuntimeConfig::load: YAML Parser exception: %1%") % e.what());
	} catch(YAML::BadDereference &e) {
		glob.log.error(boost::format("RuntimeConfig::load: YAML Bad Dereference: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("RuntimeConfig::load: YAML Map key not found: %1%") % e.what());
	} catch(YAML::RepresentationException &e) {
		glob.log.error(boost::format("RuntimeConfig::load: YAML Representation exception: %1%") % e.what());
	} catch(YAML::Exception &e) {
		glob.log.error(boost::format("RuntimeConfig::load: YAML exception: %1%") % e.what());
	} catch(...) {
		glob.log.error("RuntimeConfig::load: Generic exception caught");
	}

	return success;
}

/// loads config strings
/** This function loads the string section of the config file
	@param node A YAML::Node pointing to the string data in the config file
	\return true if the data loaded properly
*/
bool RuntimeConfig::loadStrings(const YAML::Node &node) {
	if(YAML::IsNull(node)) {
		// nothing to load here
		return true;
	}

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("RuntimeConfig::loadStrings: Node is not a map");
		return false;
	}

	std::string key, value;

	for(YAML::Iterator it = node.begin(); it != node.end(); ++it) {
		it.first() >> key;
		it.second() >> value;

		if(!mConfigStrings.insert(std::make_pair(key, value)).second) {
			glob.log.error(boost::format("RuntimeConfig::loadStrings: Cannot create map values with %1% and %2%") % key % value);
			return false;
		}
	}

	return true;
}

/// loads config integers
/** This function loads the integer section of the config file
	@param node A YAML::Node pointing to the integer data in the config file
	\return true if the data loaded properly
*/
bool RuntimeConfig::loadInts(const YAML::Node &node) {
	if(YAML::IsNull(node)) {
		// nothing to load here
		return true;
	}

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("RuntimeConfig::loadInts: Node is not a map");
		return false;
	}

	std::string key;
	int value;

	for(YAML::Iterator it = node.begin(); it != node.end(); ++it) {
		it.first() >> key;
		it.second() >> value;

		if(!mConfigInts.insert(std::make_pair(key, value)).second) {
			glob.log.error(boost::format("RuntimeConfig::loadInts: Cannot create map values with %1% and %2%") % key % value);
			return false;
		} else {
			glob.log.debug(boost::format("RuntimeConfig: Inserted %1%:%2%") % key % value);
		}
	}

	return true;
}

/// loads config floats
/** This function loads the floating point section of the config file
	@param node A YAML::Node pointing to the floating point data in the config file
	\return true if the data loaded properly
*/
bool RuntimeConfig::loadFloats(const YAML::Node &node) {
	if(YAML::IsNull(node)) {
		// nothing to load here
		return true;
	}

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("RuntimeConfig::loadFloats: Node is not a map");
		return false;
	}

	std::string key;
	float value;

	for(YAML::Iterator it = node.begin(); it != node.end(); ++it) {
		it.first() >> key;
		it.second() >> value;

		if(!mConfigFloats.insert(std::make_pair(key, value)).second) {
			glob.log.error(boost::format("RuntimeConfig::loadFloats: Cannot create map values with %1% and %2%") % key % value);
			return false;
		}
	}

	return true;
}

/// loads config bools
/** This function loads the bool section of the config file
	@param node A YAML::Node pointing to the boolean data in the config file
	\return true if the data loaded properly
*/
bool RuntimeConfig::loadBools(const YAML::Node &node) {
	if(YAML::IsNull(node)) {
		// nothing to load here
		return true;
	}

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("RuntimeConfig::loadBools: Node is not a map");
		return false;
	}

	std::string key;
	bool value;

	for(YAML::Iterator it = node.begin(); it != node.end(); ++it) {
		try {
			it.first() >> key;
			it.second() >> value;
		} catch(YAML::Exception &e) {
			glob.log.error(boost::format("RuntimeConfig::loadBools: YAML Exception caught: %1%") % e.what());
			return false;
		} catch(...) {
			glob.log.error("RuntimeConfig::loadBools: General exception caught");
			return false;
		}

		if(!mConfigBools.insert(std::make_pair(key, value)).second) {
			glob.log.error(boost::format("RuntimeConfig::loadBools: Cannot create map values with %1% and %2%") % key % value);
			return false;
		}
	}

	return true;
}

/// sets a string value
/** This function sets a string config value
	@param key the name of the key
	@param value the value to be assigned to the key
*/
void RuntimeConfig::set(const std::string &key, const std::string &value) {
	StringMap::iterator it;

	if((it = mConfigStrings.find(key)) == mConfigStrings.end()) {
		if(!mConfigStrings.insert(std::make_pair(key, value)).second) {
			glob.log.error(boost::format("RuntimeConfig::setString: Could not add map values %1% and %2%") % key % value);
		}
	} else {
		mConfigStrings[key] = value;
		glob.log.debug(boost::format("RuntimeConfig::setString: Changed %1% to %2%") % key % value);
	}

	save();
}

/// sets an int value
/** This function sets an integer config value
	@param key the name of the key
	@param value the value to be assigned to the key
*/
void RuntimeConfig::set(const std::string &key, const int value) {
	std::map<std::string, int>::iterator it;

	if((it = mConfigInts.find(key)) == mConfigInts.end()) {
		if(!mConfigInts.insert(std::make_pair(key, value)).second) {
			glob.log.error(boost::format("RuntimeConfig::setInt: Could not add map values %1% and %2%") % key % value);
		}
	} else {
		mConfigInts[key] = value;
		glob.log.debug(boost::format("RuntimeConfig::setInt: Changed %1% to %2%") % key % value);
	}

	save();
}

/// sets a float value
/** This function sets a floating point config value
	@param key the name of the key
	@param value the value to be assigned to the key
*/
void RuntimeConfig::set(const std::string &key, const float value) {
	std::map<std::string, float>::iterator it;

	if((it = mConfigFloats.find(key)) == mConfigFloats.end()) {
		if(!mConfigFloats.insert(std::make_pair(key, value)).second) {
			glob.log.error(boost::format("RuntimeConfig::setFloat: Could not add map values %1% and %2%") % key % value);
		}
	} else {
		mConfigFloats[key] = value;
		glob.log.debug(boost::format("RuntimeConfig::setFloat: Changed %1% to %2%") % key % value);
	}

	save();
}

/// sets a bool value
/** This function sets a boolean config value
	@param key the name of the key
	@param value the value to be assigned to the key
*/
void RuntimeConfig::set(const std::string &key, bool value) {
	std::map<std::string, bool>::iterator it;

	if((it = mConfigBools.find(key)) == mConfigBools.end()) {
		if(!mConfigBools.insert(std::make_pair(key, value)).second) {
			glob.log.error(boost::format("RuntimeConfig::setBool: Could not add map values %1% and %2%") % key % value);
		}
	} else {
		mConfigBools[key] = value;
		glob.log.debug(boost::format("RuntimeConfig::setBool: Changed %1% to %2%") % key % value);
	}

	save();
}

/// gets a string value
/** This function gets a string value
	@param the key to look for
	\return the value of the key
*/
std::string RuntimeConfig::getStringValue(const std::string &key) const {
	StringMap::const_iterator it;

	if((it = mConfigStrings.find(key)) != mConfigStrings.end()) {
		return it->second;
	} else {
		return "";
	}
}

/// gets an int value
/** This function gets an integer value
	@param the key to look for
	\return the value of the key
*/
int RuntimeConfig::getIntValue(const std::string &key) const {
	glob.log.debug(boost::format("RuntimeConfig: getIntValue: Looking for %1%") % key);
	std::map<std::string, int>::const_iterator it;

	if((it = mConfigInts.find(key)) != mConfigInts.end()) {
		return it->second;
	} else {
		return -1;
	}
}

/// gets a float value
/** This function gets a floating point value
	@param the key to look for
	\return the value of the key
*/
float RuntimeConfig::getFloatValue(const std::string &key) const {
	std::map<std::string, float>::const_iterator it;

	if((it = mConfigFloats.find(key)) != mConfigFloats.end()) {
		return it->second;
	} else {
		return -1.0;
	}
}

/// gets a bool value
/** This function gets a boolean value
	@param the key to look for
	\return the value of the key
*/
bool RuntimeConfig::getBoolValue(const std::string &key) const {
	std::map<std::string, bool>::const_iterator it;

	if((it = mConfigBools.find(key)) != mConfigBools.end()) {
		return it->second;
	} else {
		return false;
	}
}
