#ifndef MUD_ROOM_H
#define MUD_ROOM_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "mudconfig.h"
#include "physical.h"
#include "container.h"
#include "exit.h"
#include "message.h"

/// defines what a room is and its behavior
/** This class defines how a room behaves.
	\note Since it may be accessed by multiple threads, such as the save thread, process
	thread, etc, any reading/writing operations should lock the class mutex before they
	execute. Read only or const functions are not affected.
*/
class Room : public Physical, public Container {
public:
	typedef boost::shared_ptr<Room> RoomPointer;
	typedef std::map<std::string, RoomPointer> RoomList;

	Room();
	~Room();

	void setFileName(const std::string &name);
	std::string getFileName() const { return mFileName; }

	std::string getZoneName() const { return mZoneName; }
	void setZoneName(const std::string &name);

	std::string getItemOfInterest(const std::string &item) const;
	bool addItemOfInterest(const std::string &item, const std::string &text);

	std::string getFullDescription(const std::string &ignore = "") const;

	bool addExit(Exit::ExitPointer exit);

	Exit::ExitPointer getExit(const std::string &direction) const;
	std::string getExitString() const;

	bool Save();
	bool Load();
	bool Save(YAML::Emitter &out) const;
	bool Load(const YAML::Node &node);

	void processMessage(Message::MessagePointer message);

	void removePlayer(const std::string &name);

	void heartbeat();

	void setX(const int x) { mMapX = x; }
	void setY(const int y) { mMapY = y; }

	int getX() const { return mMapX; }
	int getY() const { return mMapY; }

	/// whether or not this room has changed since last save
	bool hasChanged() const { return mChanged; }

	/// let's the autosave know the room has changed and needs saving
	void flagChange() { mChanged = true; }

	void setWeather(const char weather, const int wind, Direction direction);

private:
	int mMapX;
	int mMapY;

	pthread_mutex_t mBusy;	///< mutex to lock so threads don't fight over this resource
	bool lock();
	bool unlock();

	StringMap mItemsOfInterest;	///< a map of Items of Interest
	std::vector<Exit::ExitPointer> mExits;	///< a vector of Exit objects

	std::string mFileName;
	std::string mZoneName; ///< The name of the zone this room belongs in

	void roomSave(YAML::Emitter &out) const;
	bool roomLoad(const YAML::Node &node);
	bool loadExits(const YAML::Node &node);

	void reparent();

	bool mChanged;

	char mCurrentWeather;
	int mWindStrength;
	Direction mWindDirection;

	std::string getBrief() const { return mBriefDescription; }
	std::string getVerbose() const { return mVerboseDescription; }

	std::string mBriefDescription;
	std::string mVerboseDescription;
};

#endif // MUD_ROOM_H
