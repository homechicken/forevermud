#ifndef SENTIENT
#define SENTIENT

#include <yaml-cpp/yaml.h>

#include "mudconfig.h"
#include "living.h"

/// a class that describes a sentient being
/** This class implements behavior and attributes of a sentient being
*/
class Sentient : public Living {
public:
	Sentient();
	~Sentient();

	/// gets the strength value
	int getStrength() const { return mStrength; }
	/// sets the strength value
	void setStrength(int strength) { mStrength = strength; }

	/// gets the intelligence value
	int getIntelligence() const { return mIntelligence; }
	/// sets the intelligence value
	void setIntelligence(int intelligence) { mIntelligence = intelligence; }

	/// gets the wisdom value
	int getWisdom() const { return mWisdom; }
	/// sets the wisdom value
	void setWisdom(int wisdom) { mWisdom = wisdom; }

	/// gets the dexterity value
	int getDexterity() const { return mDexterity; }
	/// sets the dexterity value
	void setDexterity(int dexterity) { mDexterity = dexterity; }

	/// gets the constitution value
	int getConstitution() const { return mConstitution; }
	/// sets the constitution value
	void setConstitution(int constitution) { mConstitution = constitution; }

	/// gets the charisma value
	int getCharisma() const { return mCharisma; }
	/// sets the charisma value
	void setCharisma(int charisma) { mCharisma = charisma; }

	bool addLanguage(const std::string &language, int fluency);

	void removeLanguage(const std::string &language);

	int checkLanguage(const std::string &language) const;

	void sentientSave(YAML::Emitter &out) const;
	bool sentientLoad(const YAML::Node &node);

private:
	int mStrength;	///< how strong is it
	int mIntelligence;	///< how smart is it
	int mWisdom;	///< how wise is it
	int mDexterity;	///< how agile is it
	int mConstitution;	///< how healthy is it
	int mCharisma;	///< how charming is it

	std::map<std::string, int> mLanguages;
};

#endif // SENTIENT
