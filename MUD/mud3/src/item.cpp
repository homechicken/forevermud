#include "item.h"
#include "global.h"

extern Global glob;

/// Constructor
/** This constructor sets many default values for the item. They are almost certainly
	wrong and should be fixed when the item loads.
*/
Item::Item() {
	mState = Invalid;
	mSolidToLiquidTemp = 1;
	mLiquidToGasTemp = 2;
	mGasToPlasmaTemp = 3;

	mDestroyedIfNotSolid = false;
	mMessageOnStateChange = false;

	mMagneticStrength = 0;

	mQuality = 0;
	mMessage = "";

}

/// Destructor
/** Does nothing
*/
Item::~Item() {
}
/// private function to make changes to the PhysicalState of the Item.
/** This function is for class-internal use to change the PhysicalState of the item.
	If the mMessageOnStateChange == true, a message is constructed and stored in
	mMessage. If you were to use this to force a state change, the temperatures would
	be wrong, so don't.
	@param newState the new PhysicalState the item should be in
*/
void Item::changeState(ItemPhysicalState newState) {
	if(newState == mState) {
		return;
	}

	if(mMessageOnStateChange) {
		std::stringstream s;

		s << "Changing state from ";

		switch(mState) {
		case Invalid: s << "Invalid"; break;
		case Destruct: s << "Destruct"; break;
		case Solid: s << "Solid"; break;
		case Liquid: s << "Liquid"; break;
		case Gas: s << "Gas"; break;
		case Plasma: s << "Plasma"; break;
		default: s << "NULL!";
		}

		s << " to ";

		switch(newState) {
			case Invalid: s << "Invalid"; break;
			case Destruct: s << "Destruct"; break;
			case Solid: s << "Solid"; break;
			case Liquid: s << "Liquid"; break;
			case Gas: s << "Gas"; break;
			case Plasma: s << "Plasma"; break;
			default: s << "NULL!";
		}

		s << std::endl;
		mMessage += s.str();
	}

	if(mDestroyedIfNotSolid && newState != Solid) {
		mState = Destruct;
		mMessage = "Item destructing because it's leaving the solid state";
	} else {
		mState = newState;
	}
}

/// checks item temperature and changes state if necessary
/** This function looks at the item's temperature and decides if a state change
	is necessary.
*/
void Item::checkTemperature(const int temp) {
	if(temp > mGasToPlasmaTemp) {
		if(mState != Plasma) {
			changeState(Plasma);
		}
	} else if(temp > mLiquidToGasTemp) {
		if(mState != Gas) {
			changeState(Gas);
		}
	} else if(temp > mSolidToLiquidTemp) {
		if(mState != Liquid) {
			changeState(Liquid);
		}
	} else {
		if(mState != Solid) {
			changeState(Solid);
		}
	}
}

/// gets a message if one is waiting
/** This function fetches any messages currently waiting to be retrieved.
	@param[out] msg a non-const reference to an empty string
	\return true if a message was assigned
*/
bool Item::getMessages(std::string &msg) {
	if(mMessage.empty()) {
		return false;
	}

	msg = mMessage;
	mMessage = "";

	return true;
}

std::string Item::getItemPhysicalStateString() const {
	return getItemPhysicalStateString(mState);
}

std::string Item::getItemPhysicalStateString(ItemPhysicalState state) const {
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

void Item::setItemPhysicalState(const std::string &text) {
	mState = Invalid;

	if(text == "destruct") {
		mState = Destruct;
	} else if(text == "solid") {
		mState = Solid;
	} else if(text == "liquid") {
		mState = Liquid;
	} else if(text == "gas") {
		mState = Gas;
	} else if(text == "plasma") {
		mState = Plasma;
	}
}

void Item::itemSave(YAML::Emitter &out) const {
	out << YAML::Key << "Item" << YAML::Value << YAML::BeginMap;

	out << YAML::Key << "state" << YAML::Value << getItemPhysicalStateString();
	out << YAML::Key << "messageOnChange" << YAML::Value << mMessageOnStateChange;
	out << YAML::Key << "solidToLiquidTemp" << YAML::Value << mSolidToLiquidTemp;
	out << YAML::Key << "liquidToGasTemp" << YAML::Value << mLiquidToGasTemp;
	out << YAML::Key << "gasToPlasmaTemp" << YAML::Value << mGasToPlasmaTemp;
	out << YAML::Key << "destroyedIfNotSolid" << YAML::Value << mDestroyedIfNotSolid;
	out << YAML::Key << "magneticStrength" << YAML::Value << mMagneticStrength;
	out << YAML::Key << "quality" << YAML::Value << mQuality;

	out << YAML::Key << "message" << YAML::Value << mMessage;

	if(mMessage.empty()) {
		out << YAML::Null;
	} else {
		out << mMessage;
	}

	out << YAML::EndMap;
}

bool Item::itemLoad(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Item::itemLoad(): Received a non-map YAML node");
		return success;
	}

	try {
		std::string str;

		node["state"] >> str;
		setItemPhysicalState(str);

		node["messageOnChange"] >> mMessageOnStateChange;
		node["solidToLiquidTemp"] >> mSolidToLiquidTemp;
		node["liquidToGasTemp"] >> mLiquidToGasTemp;
		node["gasToPlasmaTemp"] >> mGasToPlasmaTemp;
		node["destroyedIfNotSolid"] >> mDestroyedIfNotSolid;
		node["magneticStrength"] >> mMagneticStrength;
		node["quality"] >> mQuality;
		node["message"] >> mMessage;

		success = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Item::itemLoad(): YAML parser exception caught: %1%") % e.what());
		success = false;
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Item::itemLoad(): YAML exception caught (no such map key!): %1%") % e.what());
		success = false;
	} catch(...) {
		glob.log.error("Item::itemLoad(): caught a fall-through exception");
		success = false;
	}

	return success;
}






