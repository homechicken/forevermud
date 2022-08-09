#include "physical.h"
#include "utility.h"

#include "global.h"
extern Global glob;

#include "uuid.h"

/// Constructor
/** The constructor sets many default values that should be overridden when the actual
	object's data is loaded. This class will always be inherited, and never stand on its
	own.
	\note A object's default location is the lost and found room/zone.
*/
Physical::Physical() {
	mId = UUID::Instance()->createNull();
	mLength = 0;
	mWidth = 0;
	mHeight = 0;
	mWeight = 0;
	mTemperature = 65;
	mInsulation = 0;
	mName = "Not Initialized";
	mObjectLocation.type = RoomObject;

	mState = Solid;
	mSolidToLiquidTemp = 400;
	mLiquidToGasTemp = 600;
	mGasToPlasmaTemp = 1000;
	mDestroyedIfNotSolid = true;
	mMagneticStrength = 0;

	std::string lfRoom, lfZone;
	lfRoom = glob.Config.getStringValue("LostAndFoundRoom");
	lfZone = glob.Config.getStringValue("LostAndFoundZone");

	if(lfRoom.empty() || lfZone.empty()) {
		glob.log.error("Physical Constructor: No Lost and found location configured! This is not good.");
	}

	mObjectLocation.location = lfRoom;
	mObjectLocation.zone = lfZone;
}

/// adds a condition to this object
/** This function adds a condition to the object. If the condition is added a second
	time, it currently does nothing.
	@param cond the name of the condition to apply
	\return true if the condition wasn't already in place
	\note Conditions are simple strings to be checked for in other places. Some examples
		are OnFire, Infected, Poisoned, etc.
	\todo if the condition already exists, extend the expiration time?
*/
bool Physical::addCondition(const std::string &cond) {
	std::string fixed = Utility::toLower(cond);
	if(hasCondition(fixed)) {
		// condition already exists!
		return false;
	}
	mConditions.push_back(fixed);
	return true;
}

/// checks to see if a condition applies to this object
/** This function checks the condition list to see if a specific condition applies
	to the object.
	@param cond the name of the condition to check for
	\return true if the condition exists
*/
bool Physical::hasCondition(const std::string &cond) const {
	std::string fixed = Utility::toLower(cond);

	StringVector::const_iterator it;
	it = std::find(mConditions.begin(), mConditions.end(), fixed);
	if(it != mConditions.end()) {
		return true;
	}
	return false;
}

/// removes a condition from the object
/** This function removes a current condition from the object
	@param cond the name of the condition to remove
	\return true if the condition was found and removed
*/
bool Physical::removeCondition(const std::string &cond) {
	std::string fixed = Utility::toLower(cond);

	StringVector::iterator it;
	it = std::find(mConditions.begin(), mConditions.end(), fixed);
	if(it != mConditions.end()) {
		mConditions.erase(it);
		return true;
	}
	return false;
}

/// checks the object's nicknames for a match
/** This function is used to help identify objects, so they can be referred to as
	something other than their full, exact brief description. If not, to pick up an
	object called <b>a pointy sword</b> you would have to say <b>get a pointy sword</b>
	instead of just <b>get sword</b>
	@param name the nickname to check for
	\return true if the object responds to the name
	\see Container::containerFind()
*/
bool Physical::areYouA(const std::string &name) const {
	for(unsigned int i = 0; i < mNicknames.size(); ++i) {
		if(Utility::iCompare(mNicknames[i], name)) {
			return true;
		}
	}
	return false;
}

void Physical::addNickname(const std::string &name) {
	if(!areYouA(name)) {
		mNicknames.push_back(name);
	}
}

/// saves the data for this class
/** This function writes the internal data for this class to a YAML::Emitter. The Emitter
	should be passed in and should be inside a YAML::BeginMap
	@param out A YAML::Emitter in the Map state
*/
void Physical::physicalSave(YAML::Emitter &out) const {
	out << YAML::Key << "Physical" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "name" << YAML::Value << mName;
	out << YAML::Key << "object-type" << YAML::Value << Utility::getObjectTypeString(mObjectType);
	out << YAML::Key << "guid" << YAML::Value << UUID::Instance()->toString(mId);
	out << YAML::Key << "length" << YAML::Value << static_cast<int>(mLength);
	out << YAML::Key << "width" << YAML::Value << static_cast<int>(mWidth);
	out << YAML::Key << "height" << YAML::Value << static_cast<int>(mHeight);
	out << YAML::Key << "weight" << YAML::Value << static_cast<int>(mWeight);
	out << YAML::Key << "temperature" << YAML::Value << mTemperature;
	out << YAML::Key << "insulation" << YAML::Value << mInsulation;

	out << YAML::Key << "location" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "type" << YAML::Value << Utility::getObjectTypeString(mObjectLocation.type);
	out << YAML::Key << "name" << YAML::Value << mObjectLocation.location;
	out << YAML::Key << "zone" << YAML::Value;

	if(mObjectLocation.zone.empty()) {
		out << YAML::Null;
	} else {
		out << mObjectLocation.zone;
	}

	out << YAML::EndMap;

	out << YAML::Key << "matter-state" << YAML::Value << getMatterStateString();
	out << YAML::Key << "solid-to-liquid-temp" << YAML::Value << mSolidToLiquidTemp;
	out << YAML::Key << "liquid-to-gas-temp" << YAML::Value << mLiquidToGasTemp;
	out << YAML::Key << "gas-to-plasma-temp" << YAML::Value << mGasToPlasmaTemp;
	out << YAML::Key << "destroyed-if-not-solid" << YAML::Value << mDestroyedIfNotSolid;

	out << YAML::Key << "magnetic-strength" << YAML::Value << mMagneticStrength;

	StringVector::const_iterator it;

	out << YAML::Key << "nicknames" << YAML::Value << YAML::BeginSeq;

	if(mNicknames.size() > 0) {
		for(it = mNicknames.begin(); it != mNicknames.end(); ++it) {
			out << *it;
		}
	} else {
		out << YAML::Null;
	}
	out << YAML::EndSeq;

	out << YAML::Key << "conditions" << YAML::Value << YAML::BeginSeq;
	if(mConditions.size() > 0) {
		for(it = mConditions.begin(); it != mConditions.end(); ++it) {
			out << *it;
		}
	} else {
		out << YAML::Null;
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;
}

/// loads physical class data from a YAML::Node
/** This function loads previously saved data from this class.
	@param node A YAML::Node containing the physical save data
	\return true if the class was able to load properly
*/
bool Physical::physicalLoad(const YAML::Node &node) {
	/// \todo YAML lookup by node-name is order n^2 over the entire file. Make this loadCompat() and create a faster load()
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Container::containerLoad(): Received non-map YAML node!");
		return success;
	}

	try {
		node["name"] >> mName;

		std::string str;
		node["object-type"] >> str;
		mObjectType = Utility::getObjectTypeFromString(str);

		if(const YAML::Node *pNode = node.FindValue("guid")) {
			std::string uuidString;
			*pNode >> uuidString;
			mId = UUID::Instance()->createFromString(uuidString);
		}

		if(UUID::Instance()->isNull(mId)) {
			mId = UUID::Instance()->create();
			glob.log.warn(boost::format("Object %1% had missing guid, generated %2%") % mName % UUID::Instance()->toString(mId));
		}

		node["length"] >> mLength;
		node["width"] >> mWidth;
		node["height"] >> mHeight;
		node["weight"] >> mWeight;
		node["temperature"] >> mTemperature;
		node["insulation"] >> mInsulation;

		node["location"]["type"] >> str;
		mObjectLocation.type = Utility::getObjectTypeFromString(str);
		node["location"]["name"] >> mObjectLocation.location;

		if(!YAML::IsNull(node["location"]["zone"])) {
			node["location"]["zone"] >> mObjectLocation.zone;
		}

		if(const YAML::Node *pName = node.FindValue("matter-state")) {
			std::string state;
			*pName >> state;
			setMatterState(state);
		}

		if(const YAML::Node *pName = node.FindValue("solid-to-liquid-temp")) {
			*pName >> mSolidToLiquidTemp;
		}

		if(const YAML::Node *pName = node.FindValue("liquid-to-gas-temp")) {
			*pName >> mLiquidToGasTemp;
		}

		if(const YAML::Node *pName = node.FindValue("gas-to-plasma-temp")) {
			*pName >> mGasToPlasmaTemp;
		}

		if(const YAML::Node *pName = node.FindValue("destroyed-if-not-solid")) {
			*pName >> mDestroyedIfNotSolid;
		}

		if(const YAML::Node *pName = node.FindValue("magnetic-strength")) {
			*pName >> mMagneticStrength;
		}

		const YAML::Node &nicks = node["nicknames"];
		for(YAML::Iterator it = nicks.begin(); it != nicks.end(); ++it) {
			if(YAML::IsNull(*it)) {
				break;
			}
			std::string scalar;
			*it >> scalar;
			addNickname(scalar);
		}

		const YAML::Node &conds = node["conditions"];
		for(YAML::Iterator it = conds.begin(); it != conds.end(); ++it) {
			if(YAML::IsNull(*it)) {
				break;
			}
			std::string scalar;
			*it >> scalar;
			mConditions.push_back(scalar);
		}

		success = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Physical::physicalLoad(): YAML parser exception caught: %1%") % e.what());
		success = false;
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Physical::physicalLoad(): YAML exception caught (no such map key!): %1%") % e.what());
		success = false;
	} catch(...) {
		glob.log.error("Physical::physicalLoad(): caught a fall-through exception");
		success = false;
	}

	return success;
}

/// gradually reconciles the item temperature with the ambient temperature
/** This function is simple, but cool. Any item can easily be warmed or cooled
	depending on the ambient temperature of the container it resides in. If an item
	is insulated it will prolong the temperature change, but not forever.
	@param temp the ambient temperature to be equalized with
	\note Temperatures will be equalized from the environment that the derived object
	is located in, but it is up to the derived object to determine what to do at what
	temperature.
*/
void Physical::equalizeTemperature(const int temp) {
	if(temp == mTemperature) {
		return;
	}

	int difference = temp - mTemperature;
	int delta = 0;

	if(difference > 0) {
		// we need to warm up
		if(difference > 10) {
			delta = 5 - mInsulation;
			if(delta < 1) {
				delta = 1;
			}
		} else if(difference > 100) {
			delta = 20 - mInsulation;
			if(delta < 1) {
				delta = 1;
			}
		} else {
			delta = 1;
		}
	} else {
		// we need to cool off
		if(difference < -10) {
			delta = -5 + mInsulation;
			if(delta > -1) {
				delta = -1;
			}
		} else if(difference < -100) {
			delta = -20 + mInsulation;
			if(delta > -1) {
				delta = -1;
			}
		} else {
			delta = -1;
		}
	}

	mTemperature += delta;

	checkMatterState();
}

std::string Physical::getMatterStateString() const {
	return getMatterStateString(mState);
}

std::string Physical::getMatterStateString(MatterState state) const {
	std::string s;

	switch(state) {
		case Destruct: s = "destruct"; break;
		case Solid: s = "solid"; break;
		case Liquid: s = "liquid"; break;
		case Gas: s = "gas"; break;
		case Plasma: s = "plasma"; break;
		default: s = "invalid";
	}

	return s;
}

void Physical::setMatterState(const std::string &state) {
	mState = Invalid;

	if(state == "destruct") {
		mState = Destruct;
	} else if(state == "solid") {
		mState = Solid;
	} else if(state == "liquid") {
		mState = Liquid;
	} else if(state == "gas") {
		mState = Gas;
	} else if(state == "plasma") {
		mState = Plasma;
	}
}

void Physical::checkMatterState() {
	if(mTemperature > mGasToPlasmaTemp) {
		if(mState != Plasma) {
			changeState(Plasma);
		}
	} else if(mTemperature > mLiquidToGasTemp) {
		if(mState != Gas) {
			changeState(Gas);
		}
	} else if(mTemperature > mSolidToLiquidTemp) {
		if(mState != Liquid) {
			changeState(Liquid);
		}
	} else {
		if(mState != Solid) {
			changeState(Solid);
		}
	}
}

void Physical::changeState(MatterState state) {
	if(state == mState) {
		return;
	}

	if(mDestroyedIfNotSolid && mState != Solid) {
		mState = Destruct;
	} else {
		mState = state;
	}
}

