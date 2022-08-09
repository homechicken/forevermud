#include "exit.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** This function sets some default values for exits. The values are:
	\c No door
	\c Not closed
	\c Not lockable
	\c Not locked
	\c Not hidden
*/
Exit::Exit() {
	mHasDoor = false;
	mClosed = false;
	mLocked = false;
	mLockable = false;
	mHidden = false;
}

/// Destructor
/** Nothing happens here
*/
Exit::~Exit() {
}

/// locks this door if possible
/** This function locks the door
	@param keyName the name of the key that is trying to lock the door
	\return true if they key can lock the door
	\todo to not be annoying, we should probably close the door if it's open
*/
bool Exit::lock(const std::string &keyName) {
	if(mHasDoor && mClosed && !mLocked) {
		if(Utility::iCompare(keyName, mKey)) {
			mLocked = true;
			return true;
		}
	}
	return false;
}

/// unlocks the door if possible
/** This function unlocks the door if you provide the right key
	@param keyName the name of the key that is trying to unlock the door
	\return true if the key unlocks the door
	\note This function returns false if there is a door and it is already unlocked
*/
bool Exit::unlock(const std::string &keyName) {
	if(mHasDoor && mClosed && mLocked) {
		if(Utility::iCompare(keyName, mKey)) {
			mLocked = false;
			return true;
		}
	}
	return false;
}

/// closes the door
/** This function closes the door if possible
	\return true if the door was closed by this command, false if no door or it was closed already
*/
bool Exit::close() {
	if(mHasDoor && !mClosed) {
		mClosed = true;
		return true;
	}
	return false;
}

/// opens the door if possible
/** This function opens the door if possible
	\return true if the door was opened by this command, false if no door or it was already open
*/
bool Exit::open() {
	if(mHasDoor && mClosed && !mLocked) {
		mClosed = false;
		return true;
	}
	return false;
}

/// find out if someone can go through this exit
/** This function checks the Exit to see if a person can pass through it
	\return true if it can be passed through
*/
bool Exit::canPass() const {
	// by passing a Player::PlayerPointer here, we could check conditions
	// or race, etc to determine if someone can pass through...
	if(!mHasDoor || (mHasDoor && !mClosed)) {
		return true;
	}
	return false;
}

/// saves the exit data
/** This function saves the exit data so it may be reloaded from storage
	later.
	@param out A YAML::Emitter in the sequence state
*/
void Exit::exitSave(YAML::Emitter &out) const {
	out << YAML::BeginMap;

	out << YAML::Key << "name" << YAML::Value << mName;
	out << YAML::Key << "destination-zone" << YAML::Value << mDestinationZone;
	out << YAML::Key << "destination" << YAML::Value << mDestination;

	out << YAML::Key << "key" << YAML::Value;
	if(mKey.empty()) {
		out << YAML::Null;
	} else {
		out << mKey;
	}

	out << YAML::Key << "lockable" << YAML::Value << mLockable;
	out << YAML::Key << "has-door" << YAML::Value << mHasDoor;
	out << YAML::Key << "closed" << YAML::Value << mClosed;
	out << YAML::Key << "hidden" << YAML::Value << mHidden;

	out << YAML::EndMap;
}

/// loads an exit
/** This function reloads a saved exit from a string.
	@param node a YAML::Node pointing to the saved Exit data
	\return true if the exit loaded properly
*/
bool Exit::exitLoad(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Exit::exitLoad(): Node is not a map!");
		return success;
	}

	try {
		node["name"] >> mName;
		node["destination-zone"] >> mDestinationZone;
		node["destination"] >> mDestination;

		if(!YAML::IsNull(node["key"])) {
			node["key"] >> mKey;
		}

		node["lockable"] >> mLockable;
		node["has-door"] >> mHasDoor;
		node["closed"] >> mClosed;
		node["hidden"] >> mHidden;

		success = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Player::playerLoad: YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Player::playerLoad: YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Player::playerLoad: caught a fall-through exception");
	}

	return success;
}
