#ifndef MUD_EXIT_H
#define MUD_EXIT_H

#include <boost/shared_ptr.hpp>
#include "mudconfig.h"
#include "player.h"

/// handles all exits from a room
/** This class is used to define what exits exist (hehe) in a room and to where
	they lead. It also controls whether doors are present, closed, or locked.
*/
class Exit {
public:
	/// shortcut to make shared pointers easier to write
	typedef boost::shared_ptr<Exit> ExitPointer;

	Exit();
	~Exit();

	/// gets the name of this exit
	std::string getName() const { return mName; }

	/// sets the name of this exit
	void setName(const std::string &name) { mName = name; }

	/// gets the destination to where this exit leads
	std::string getDestination() const { return mDestination; }

	/// gets the destination zone to where this exit leads
	std::string getDestinationZone() const { return mDestinationZone; }

	/// sets the destination this exit leads to
	void setDestination(const std::string &dest) { mDestination = dest; }

	/// sets the desigination zone where this exit leads
	void setDestinationZone(const std::string &zone) { mDestinationZone = zone; }

	/// returns the name of the key object that unlocks this door
	std::string getKey() const { return mKey; }

	/// sets the name of the key object that unlocks this door
	void setKey(const std::string &key) { mKey = key; }

	/// returns true if this door has a lock
	bool isLockable() const { return mLockable; }

	/// defines whether this door can be locked or not
	void setLockable(bool canLock) { mLockable = canLock; }

	/// returns whether this door is locked or not
	bool isLocked() const { return mLocked; }

	/// sets whether the door is locked or not
	void setLocked(bool locked) { mLocked = locked; }

	bool lock(const std::string &keyName);
	bool unlock(const std::string &keyName);

	bool close();
	bool open();

	/// returns whether or not this exit has a door
	bool hasDoor() const { return mHasDoor; }

	/// defines whether this exit has a door to go through
	void setHasDoor(bool door) { mHasDoor = door; }

	/// returns whether or not the door is closed
	bool isClosed() const { return mClosed; }

	/// sets whether the door is closed or not
	void setClosed(bool closed) { mClosed = closed; }

	bool canPass() const;

	/// tells you if this exit is hidden and shouldn't be listed normally
	bool isHidden() const { return mHidden; }

	/// makes this exit hidden so it doesn't appear in the normal exit list
	void setHidden(bool hide) { mHidden = hide; }

	void exitSave(YAML::Emitter &out) const;
	bool exitLoad(const YAML::Node &node);

private:
	std::string mName;	///< the name of this exit
	std::string mDestination;	///< the name of the room to which this exit leads
	std::string mDestinationZone; ///< the name of the zone where the exit room leads to
	std::string mKey;	///< the name of the key that unlocks this door

	bool mLockable;	///< true if this door can be locked
	bool mLocked;	///< true if this door is locked
	bool mHasDoor;	///< true if this exit has a door
	bool mClosed;	///< true if this door is closed
	bool mHidden;	///< true if this exit is a secret

};

#endif // MUD_EXIT_H
