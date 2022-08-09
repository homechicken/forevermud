#ifndef ZONE
#define ZONE

#include <boost/shared_ptr.hpp>
#include <string>

#include "mudconfig.h"
#include "zoneMap.h"
#include "room.h"

/// Handles all data associated with a game zone
/** This class keeps all data and functions together for a single zone in the game.
*/
class Zone {
public:
	typedef boost::shared_ptr<Zone> ZonePointer;			///< a boost::shared_ptr shortened to make it easier to instantiate
	typedef std::map<std::string, ZonePointer> ZoneList;	///< a map of zone name to zone shared_ptrs

	Zone();
	~Zone();

	/// sets the name of the zone
	void setName(const std::string &name) { mZoneName = name; }

	/// gets the name of the zone
	std::string getName() const { return mZoneName; }

	void load();

	void heartbeat();

	void addRoom(const std::string &roomName);

	Room::RoomPointer getRoom(const std::string &roomName) const;

	void validateExits();
	void saveAll(bool force = true); /// \todo change this back to false when rooms have updated physical stats

	/// how many rooms are in this zone?
	unsigned int getNumberOfRooms() const { return mRoomList.size(); }

	/// whether or not this zone has a map associated with it
	bool hasMap() const { return mHasMap; }

	std::string getRadiusMap(const unsigned int x, const unsigned int y, const unsigned int radius, bool showLegend) const;

	/// pass-through function for getting the weather in a room
	std::string getWeatherForRoom(const int x, const int y) const { return mZoneMap.getWeatherString(x, y); }

	/// pass-through function for getting the wind in a room
	std::string getWindString() const { return mZoneMap.getWindString(); }

	bool hasWeather() const { return mZoneMap.hasWeather(); }

private:
	std::string mZoneName;		///< the name of the zone
	ZoneMap mZoneMap;			///< the map object for this zone, if applicable
	Room::RoomList mRoomList;	///< a list of rooms belonging to this zone

	bool mHasMap;	///< if this zone has a map

	void loadMaps();
	void loadRooms();

	void consistencyCheck();

	void handleWeather();
	void updateRoomWeather();

	unsigned int mMinimumStableWindPeriod;	///< the least amount of time before which the weather can change
	unsigned int mTimeSinceLastWindChange;	///< how long it's been since we last changed the wind direction
	unsigned int mTimeSinceLastWeatherMapShift;	///< how long it's been since we last shifted the weather map
};


#endif // ZONE
