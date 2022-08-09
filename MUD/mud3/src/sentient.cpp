#include "sentient.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** The constructor sets default values for attributes that should be overloaded
	when the inheritor loads.
*/
Sentient::Sentient() {
	mStrength = 1;
	mIntelligence = 1;
	mWisdom = 1;
	mDexterity = 1;
	mConstitution = 1;
	mCharisma = 1;
}

/// Destructor
/** Does nothing
*/
Sentient::~Sentient() {
}

/// saves the class data
/** This function writes out the class data in YAML format.
	@param out A YAML::Emitter to write out to
*/
void Sentient::sentientSave(YAML::Emitter &out) const {
	out << YAML::Key << "Sentient" << YAML::Value << YAML::BeginMap;

	out << YAML::Key << "strength" << YAML::Value << mStrength;
	out << YAML::Key << "intelligence" << YAML::Value << mIntelligence;
	out << YAML::Key << "wisdom" << YAML::Value << mWisdom;
	out << YAML::Key << "dexterity" << YAML::Value << mDexterity;
	out << YAML::Key << "constitution" << YAML::Value << mConstitution;
	out << YAML::Key << "charisma" << YAML::Value << mCharisma;

	out << YAML::Key << "languages" << YAML::Value;

	if(mLanguages.size() > 0) {
		out << YAML::BeginMap;

		for(std::map<std::string, int>::const_iterator it = mLanguages.begin(); it != mLanguages.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
		}

		out << YAML::EndMap;
	} else {
		out << YAML::Null;
	}

	out << YAML::EndMap;
}

/// loads class data
/** This function reloads previously saved data
	@param node A YAML::Node (must be a map) where the Sentient data begins
	\return true if the class loaded without errors
*/
bool Sentient::sentientLoad(const YAML::Node &node) {
	bool retval = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.debug("Sentient::sentientLoad(): Node is not a map");
		return retval;
	}

	try {
		node["strength"] >> mStrength;
		node["intelligence"] >> mIntelligence;
		node["wisdom"] >> mWisdom;
		node["dexterity"] >> mDexterity;
		node["constitution"] >> mConstitution;
		node["charisma"] >> mCharisma;

		if(!YAML::IsNull(node["languages"])) {
			const YAML::Node &langs = node["languages"];
			for(YAML::Iterator it = langs.begin(); it != langs.end(); ++it) {
				std::string language;
				int fluency;

				it.first() >> language;
				it.second() >> fluency;

				if(!addLanguage(Utility::toLower(language), fluency)) {
					glob.log.error(boost::format("Sentient::sentientLoad(): Could not add language %1% with fluency %2%") % language % fluency);
				}
			}
		}

		retval = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Sentient::sentientLoad(): YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Sentient::sentientLoad(): YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Sentient::sentientLoad(): caught a fall-through exception");
	}

	return retval;
}

/// how fluently does this being speak this language?
/** This function checks the class data for a language and returns the degree of
	fluency for it.
	@param language The language to check for
	\return The being's fluency in this language (0-100)
*/
int Sentient::checkLanguage(const std::string &language) const {
	std::map<std::string, int>::const_iterator it;

	if((it = mLanguages.find(language)) != mLanguages.end()) {
		return it->second;
	}

	return 0;
}

/// removes a language from the list
/** This function removes the specified language from the being's list of recognized
	languages.
	@param language The language to remove
	\note logs an error but silently fails if the language was not present
*/
void Sentient::removeLanguage(const std::string &language) {
	std::map<std::string, int>::iterator it;

	if((it = mLanguages.find(language)) != mLanguages.end()) {
		mLanguages.erase(it);
	} else {
		glob.log.warn(boost::format("Sentient::removeLanguage(): Had no language '%1%' to remove!") % language);
	}
}

/// adds a language to the list
/** This function adds a specified language to the list the being understands with
	a fluency rating.
	@param language The language to add
	@param fluency The percentage of fluency in this language (0-100)
*/
bool Sentient::addLanguage(const std::string &language, int fluency) {
	bool success = false;

	if(fluency < 0) {
		fluency = 0;
	}

	if(fluency > 100) {
		fluency = 100;
	}

	if(mLanguages.insert(std::make_pair(language, fluency)).second) {
		success = true;
	}

	return success;
}
