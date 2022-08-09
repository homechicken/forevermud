#include <sstream>
#include "socialData.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
SocialData::SocialData() {
}

/// Destructor
/** Does nothing
*/
SocialData::~SocialData() {
}

/// reassigns pronouns according to the sex of the issuer
/** This function reconfigures the internal pronouns according to the sex of the
	player calling the social.
	@param sex the sex of the player issuing the social
*/
void SocialData::configureFor(Living::Sex sex) {
	switch(sex) {
		case Living::Male:
			subjective = "he";
			objective = "him";
			possessive = "his";
			reflexive = "himself";
			break;
		case Living::Female:
			subjective = "she";
			objective = "her";
			possessive = "hers";
			reflexive = "herself";
			break;
		case Living::Neuter:
			subjective = "it";
			objective = "it";
			possessive = "its";
			reflexive = "itself";
			break;
		default:
			subjective = "non";
			objective = "non";
			possessive = "nons";
			reflexive = "non-self";
	}
}

/// gets self-targeted Singular version of the social
/** This function returns the part of the singular social (eg no target named)
	that the issuing player sees him/herself.
	\return the singular self version of the social
*/
std::string SocialData::getSingularSelf() {
	return singularSelf;
}

/// generates the singular form of the Social that others will see
/** This function generates the singular form of the social that everyone else
	sees.
	@param me the name of the player issuing the social
	\return the singular others form of the social
*/
std::string SocialData::getSingularOthers(const std::string &me) {
	return replace(singularOthers, me, "");
}

/// generates the plural form of the social that the issuing player sees
/** This function generates the plural form (the form called \e with a target) that
	the player issuing the social sees.
	@param target the name of the object that is the target of the social
	\return the plural self form of the social
*/
std::string SocialData::getPluralSelf(const std::string &target) {
	return replace(pluralSelf, "", target);
}

/// generates the plural form of the social that the target sees
/** This function generates the plural form of the social that the targetted object
	sees.
	@param me the name of the player issuing the social
	@param target the name of the object that is the target of the social
	\return the plural target version of the social
*/
std::string SocialData::getPluralTarget(const std::string &me, const std::string &target) {
	return replace(pluralTarget, me, target);
}

/// generates plural form of the social that everyone but the issuing player and target sees
/** This function generates the form of the social that everyone that is not directly involved
	with the social sees.
	@param me the name of the player issuing the social
	@param target the name of the object that is the target of the social
	\return the plural others form of the social
*/
std::string SocialData::getPluralOthers(const std::string &me, const std::string &target) {
	return replace(pluralOthers, me, target);
}

/// a helper function that swaps out variables to format the social output
/** This function replaces strings in the social output, formatting it with names in the
	right places as well as replacing the different pronouns.
	@param txt the social string
	@param me the name of the player issuing the social
	@param target the name of the object that is the target of the social
	\return a string of replaced tokens and pronouns
*/
std::string SocialData::replace(const std::string &txt, const std::string &me, const std::string &target) {
	std::string s = Utility::stringReplace(txt, "%ME%", me);
	s = Utility::stringReplace(s, "%TARGET%", target);
	s = Utility::stringReplace(s, "%SUBJECTIVE%", subjective);
	s = Utility::stringReplace(s, "%OBJECTIVE%", objective);
	s = Utility::stringReplace(s, "%POSSESSIVE%", possessive);
	s = Utility::stringReplace(s, "%REFLEXIVE%", reflexive);
	return s;
}

/// saves social data to storage
/** This function turns the social into a saveable string to be placed on the storage
	system.
	\return a string of the social that is loadable later
*/
std::string SocialData::saveToString() {
	if(name.empty()) {
		return "";
	}
	std::stringstream s;
	s << name << "\n";
	s << singularSelf << "\n";
	s << singularOthers << "\n";
	s << pluralSelf << "\n";
	s << pluralTarget << "\n";
	s << pluralOthers << "\n#\n";

	return s.str();
}

/// loads a social from text
/** This function loads social data from the provided string.
	@param saveString the string that was previously saved containing the social data
	\return true if the social was loaded properly
*/
bool SocialData::loadFromString(const std::string &saveString) {
	StringVector data = Utility::stringToVector(saveString, "\n");

	if(data.size() != 6) {
		std::stringstream s;
		s << "Social data mismatch:\n";
		for(unsigned int i = 0; i < data.size(); ++i) {
			s << data[i] << "." << std::endl;
		}
		glob.log.error(s.str());
		return false;
	}
	if(data[0].empty() || data[1].empty() ||
		data[2].empty() || data[3].empty() ||
		data[4].empty() || data[5].empty()) {
		return false;
	}
	name = data[0];
	singularSelf = data[1];
	singularOthers = data[2];
	pluralSelf = data[3];
	pluralTarget = data[4];
	pluralOthers = data[5];

	return true;
}
