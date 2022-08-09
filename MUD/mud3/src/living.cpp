#include <string>
#include "living.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** This constructor sets some default values that should be overrided when the
	Living is loaded.
*/
Living::Living() {
	mLife = 90;
	mMaxLife = 90;
	mSex = Male;
	mLivingState = Alive;
	mLivingPosture = Standing;
	mRace = "Human";
	mTempComfortLow = 65;
	mTempComfortHigh = 75;
	mTempDamageLow = 50;
	mTempDamageHigh = 110;
}

/// Destructor
/** Does nothing
*/
Living::~Living() {
}

/// adds life (heals) this Living object
/** This function adds life if the Living is alive, up to the mMaxLife value
	@param amount how much life to add
	\return true if the amount Living is alive and the amount is positive
*/
bool Living::addLife(const int amount) {
	if(mLivingState < Dead) {
		if(amount > 0) {
			mLife += amount;
			if(mLife > mMaxLife) {
				mLife = mMaxLife;
			}
			return true;
		}
	}
	return false;
}

/// sets the sex of the Living object
/** This function sets the sex of this living, and also the relevant pronouns.
	@param sex the sex of this living
*/
void Living::setSex(Sex sex) {
	mSex = sex;
	switch(mSex) {
	case Male:
		mPossessivePronoun = "his";
		break;
	case Female:
		mPossessivePronoun = "her";
		break;
	case Neuter:
	default:
		mPossessivePronoun = "its";
	}
}

/// subtracts life (damages) this Living object
/** This function subtracts life from the living object, killing it or making it
	unconscious when it reaches a tolerance (defined in mudconfig.h)
	@param amount the amount of life to subtract
	\return true if the Living was not dead
*/
bool Living::subtractLife(const int amount) {
	if(mLivingState < Dead) {
		if(amount > 0) {
			mLife -= amount;

			int death = glob.Config.getIntValue("LifepointDie");
			int unconscious = glob.Config.getIntValue("LifepointUnconscious");

			if(mLife <= death) {
				mLivingState = Dead;
			} else if(mLife <= unconscious) {
				mLivingState = Unconscious;
			}
			return true;
		}
	}
	return false;
}

/// tells you what condition the living is in
/** This function generates a string based on the percentage of mLife to mMaxLife
	for this Living object.	There are 10 states ranging from Perfect to Faint.
	\return a string describing the state of the Living object
*/
std::string Living::livingGetStateString() const {
	int tenth = Utility::toInt(mMaxLife / 10);
	int state = Utility::toInt(mLife / tenth);

	std::string s;

	switch(mLivingState) {
	case Alive:
		switch(state) {
		case 0: s = "~bwrfaint~res"; break;
		case 1: s = "~bwrfeeble~res"; break;
		case 2: s = "~br0weak~res"; break;
		case 3: s = "~ny0poor~res"; break;
		case 4: s = "~ny0okay~res"; break;
		case 5: s = "~by0good~res"; break;
		case 6: s = "~by0healthy~res"; break;
		case 7: s = "~ng0strong~res"; break;
		case 8: s = "~ng0tough~res"; break;
		case 9: s = "~bg0excellent~res"; break;
		case 10: s = "~bg0perfect~res"; break;
		default:
			glob.log.warn("Unknown state in Living::getStateString");
			s = "unknown";
		}
		break;

	case Unconscious:
		s = "unconscious";
		break;

	case Dead:
		s = "dead";
		break;

	case Ghost:
		s = "ghost";
		break;

	default:
		glob.log.error("Unknown state reached in Living::getStateString");
		s = "unknown";
	}

	return s;
}

/// brings someone back to life with 1 life
/** This function returns a dead Living to life with 1 mLife
	\return true if the Living was not alive
*/
bool Living::livingRevive() {
	if(mLivingState >= Unconscious) {
		mLife = 1;
		mLivingState = Alive;
		return true;
	}
	return false;
}

/// generates a string based on the LivingPosture value
/** This function generates a string based on what the current LivingPosture value
	is for this Living object
	\return a short string describing the posture of the Living object
*/
std::string Living::livingGetPostureString() const {
	std::string s;

	switch(mLivingPosture) {
	case Standing: s = "standing"; break;
	case Kneeling: s = "kneeling";  break;
	case Sitting: s = "sitting"; break;
	case Lying: s = "lying"; break;
	case Prone: s = "prone"; break;
	default:
		glob.log.error("Unknown posture in Living::livingGetPostureString");
		s = "unknown";
	}

	return s;
}

/// sets the LivingPosture of this object
/** This function changes the LivingPosture of this object if it is alive and conscious.
	@param posture the LivingPosture value to change to
	\return true if the object is alive and conscious
*/
bool Living::livingSetPosture(LivingPosture posture) {
	if(mLivingState > Alive) {
		return false;
	}

	mLivingPosture = posture;

	return true;
}

/// writes the bits to restore this class to the same state out to a YAML::Emitter
/** This function uses the \c yaml-cpp library to write out data that can be restored
	later. Originally I wrote out \b state and \b posture values, but decided to evaluate
	the state at load-time (whether the living is alive, etc), and reset the posture to
	a default value.
	@param out A YAML::Emitter reference to send the data out via.
*/
void Living::livingSave(YAML::Emitter &out) const {
	out << YAML::Key << "Living" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "life" << YAML::Value << mLife;
	out << YAML::Key << "max-life" << YAML::Value << mMaxLife;
	out << YAML::Key << "sex" << YAML::Value << mSex;
	out << YAML::Key << "temp-damage-low" << YAML::Value << mTempDamageLow;
	out << YAML::Key << "temp-comfort-low" << YAML::Value << mTempComfortLow;
	out << YAML::Key << "temp-comfort-high" << YAML::Value << mTempComfortHigh;
	out << YAML::Key << "temp-damage-high" << YAML::Value << mTempDamageHigh;
	out << YAML::EndMap;
}

/// Reloads data previously saved and restores the class
/** This function reloads previously saved data into the object. An attempt is made to
	protect from YAML exceptions, but if one is thrown the object may be in a bad state.
	If so, an error will be logged.
	@param node A YAML::Node Value (should be a YAML::CT_MAP) for a Living object
	\return true if the object was able to load properly
*/
bool Living::livingLoad(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Living::livingLoad(): Received non-map YAML node!");
		return false;
	}

	try {
		int sex;
		node["life"] >> mLife;
		node["max-life"] >> mMaxLife;
		node["sex"] >> sex;
		node["temp-damage-low"] >> mTempDamageLow;
		node["temp-comfort-low"] >> mTempComfortLow;
		node["temp-comfort-high"] >> mTempComfortHigh;
		node["temp-damage-high"] >> mTempDamageHigh;

		switch(sex) {
			case 0:
				mSex = Male;
				break;
			case 1:
				mSex = Female;
				break;
			case 2:
			default:
				mSex = Neuter;
		}

		int unconscious = glob.Config.getIntValue("LifepointUnconscious");
		int death = glob.Config.getIntValue("LifepointDie");

		if(mLife > unconscious) {
			mLivingState = Alive;
			mLivingPosture = Standing;
		} else {
			mLivingPosture = Lying;

			if(mLife > death) {
				mLivingState = Unconscious;
			} else {
				mLivingState = Dead;
			}
		}
		success = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Living::livingLoad: YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Living::livingLoad: YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Living::livingLoad: caught a fall-through exception");
	}

	return success;
}
