#ifndef MUD_SOCIALDATA_H
#define MUD_SOCIALDATA_H

#include <string>
#include "living.h"

/// a single social object, aka pre-packaged emotes
/** This class handles the data for a single social, or preconfigured emote.
	There are two types of socials: the Singular and the Plural.
	
	The singular socials are issued without a target, such as \c social \c wink.
	
	Plural socials have a target the social applies to, eg \c social \c wink
	\c jacob.
	
	Singular socials have two parts, the Self and the Other. Self is what the
	issuing player sees, Other is what everyone else (in the room) sees.
		
	Plural socials have an extra part, the Target, which sees a different version
	than the Plural Self and Plural Others.
	
	Put together, each social has 5 parts, the two singular and three plural. Add
	to that the name of the social, and you have the storage method I use.
	
	\note The socials are stored in data/socials
*/
class SocialData {
public:
	SocialData();
	~SocialData();

	std::string getName() { return name; }
	void setName(const std::string &set) { name = set; }
	
	void setSubjective(const std::string &set) { subjective = set; }
	std::string getSubjective() { return subjective; }
	
	void setObjective(const std::string &set) { objective = set; }
	std::string getObjective() { return objective; }
	
	void setPossessive(const std::string &set) { possessive = set; }
	std::string getPossessive() { return possessive; }
	
	std::string getReflexive() { return reflexive; }
	void setReflexive(const std::string &set) { reflexive = set; }
	
	void configureFor(Living::Sex sex);
	
	std::string getSingularSelf();
	void setSingularSelf(const std::string &set) { singularSelf = set; }
	
	std::string getSingularOthers(const std::string &me);
	void setSingularOthers(const std::string &set) { singularOthers = set; }
	
	std::string getPluralSelf(const std::string &target);
	void setPluralSelf(const std::string &set) { pluralSelf = set; }
	
	std::string getPluralTarget(const std::string &me, const std::string &target);
	void setPluralTarget(const std::string &set) { pluralTarget = set; }
	
	std::string getPluralOthers(const std::string &me, const std::string &target);
	void setPluralOthers(const std::string &set) { pluralOthers = set; }
	
	std::string saveToString();
	bool loadFromString(const std::string &saveString);
	
private:
	std::string name;
	// the different pronouns we need
	std::string subjective, objective, possessive, reflexive;

	std::string singularSelf, singularOthers;
	std::string pluralSelf, pluralTarget, pluralOthers;

	std::string replace(const std::string &txt, const std::string &me, const std::string &target);

};

#endif // MUD_SOCIALDATA_H
