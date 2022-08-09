#include "zone.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// the constructor does nothing
/** The constructor doesn't have anything to do yet.
*/
Zone::Zone() {
	mHasMap = false;
	int stableTime = glob.Config.getIntValue("MinimumStableWindPeriod");

	if(stableTime < 1) {
		stableTime = 1000;
	}

	/// \todo move this out of the constructor so it can be dynamically changed
	mMinimumStableWindPeriod = static_cast<unsigned int>(stableTime);
	mTimeSinceLastWindChange = 0;
	mTimeSinceLastWeatherMapShift = 0;
}

/// the destructor saves everything before it leaves scope
/** The destructor attempts to force all rooms to save themselves before going out of scope.
*/
Zone::~Zone() {
	saveAll(true);
}

/// Loads a zone if a name has been assigned
/** This functions should be called after assigning a zone name to this object. If so, it will attempts
	to load any associated maps and rooms for the zone in to memory. After doing so, it will also perform an
	internal consistency check.
*/
void Zone::load() {
	if(mZoneName.empty()) {
		glob.log.error("Zone::load(): Tried to load an empty zone");
		return;
	}

	loadMaps();

	glob.log.debug(boost::format("Zone::load(): Loading rooms for zone %1%") % mZoneName);

	loadRooms();

	glob.log.debug(boost::format("Zone::load(): Currently have %1% rooms loaded for zone %2%") % mRoomList.size() % mZoneName);
	consistencyCheck();
}

/// loads all maps for this zone
/** This function loads maps and keys for the zone, if available. They are not required, but are nice.
*/
void Zone::loadMaps() {
	if(mZoneName.empty()) {
		glob.log.error("Zone::loadMap(): No zone name to load map from!");
		return;
	}

	IOResourceLocator res;
	res.type = ZoneObject;
	res.meta = mZoneName;
	res.name = "map.txt";

	std::string data = glob.ioDaemon.getResource(res);

	if(data != IO_RESOURCE_NOT_FOUND) {
		mZoneMap.setName(mZoneName);
		StringVector cleanRoomData = Utility::stripCommentedLines(data);
		mZoneMap.setMap(cleanRoomData);
		mHasMap = true;

		// also check for the presence of a map key
		res.name = "map.key";
		data = glob.ioDaemon.getResource(res);

		if(data != IO_RESOURCE_NOT_FOUND) {
			StringVector cleanMapKey = Utility::stripCommentedLines(data);
			mZoneMap.setKey(cleanMapKey, "map");
		}

		// is there also a weather map?
		res.name = "weather.txt";
		data = glob.ioDaemon.getResource(res);

		if(data != IO_RESOURCE_NOT_FOUND) {
			StringVector cleanWeatherMap = Utility::stripCommentedLines(data);
			mZoneMap.setWeatherMap(cleanWeatherMap);
			mZoneMap.changeWind();

			// does the weather map have a key?
			res.name = "weather.key";
			data = glob.ioDaemon.getResource(res);

			if(data != IO_RESOURCE_NOT_FOUND) {
				StringVector cleanWeatherKey = Utility::stripCommentedLines(data);
				mZoneMap.setKey(cleanWeatherKey, "weather");
			}
		}
	}
}

/// loads rooms for the zone
/** This function loads all rooms from the zone's \c rooms directory as long as there are no errors.
*/
void Zone::loadRooms() {
	if(mZoneName.empty()) {
		glob.log.error("Zone::loadRooms(): No zone name to load rooms from!");
		return;
	}

	StringVector allRooms = glob.ioDaemon.getRoomsForZone(mZoneName);
	unsigned int numRoomsLoaded = 0;

	if(allRooms.size() > 0) {
		glob.log.info(boost::format("Zone::loadRooms(): There are %1% rooms to load for zone %2%") % allRooms.size() % mZoneName);

		for(StringVector::iterator it = allRooms.begin(); it != allRooms.end(); ++it) {
			addRoom(*it);
			++numRoomsLoaded;

			// if there are a lot of rooms in this zone, this will provide some feedback while they're loading
			if(numRoomsLoaded % 1000 == 0) {
				glob.log.info(boost::format("Zone::loadRooms(): Progress: loaded %1% rooms so far") % numRoomsLoaded);
			}
		}
	} else {
		glob.log.error(boost::format("Zone::loadRooms(): No rooms to load for zone %1%") % mZoneName);
	}

	glob.log.debug(boost::format("Zone::loadRooms(): Done loading rooms for zone %1%") % mZoneName);
}

/// adds a room to the contents of the zone
/** This function is a helper for \c loadRooms() and will attempt to load a room from a saved state.
	Rooms MUST have unique names within the zone or they will not be loaded (first one gets in). Duplicate
	names will be logged.
	\see loadRooms()
	@param roomName The name of the room to load
*/
void Zone::addRoom(const std::string &roomName) {
	if(roomName.empty()) {
		glob.log.error("Zone::addRoom received empty saved text");
		return;
	}

	Room::RoomPointer room(new Room);

	room->setZoneName(mZoneName);
	room->setFileName(roomName);
	room->setName(Utility::toProper(mZoneName));

	if(room->Load()) {
		// the lower-case file name is paired with the pointer because it is guaranteed to be unique while the room "name" is not!
		std::string lowerName = Utility::toLower(roomName);
		if(!mRoomList.insert(std::make_pair(lowerName, room)).second) {
			// error inserting room due to duplicate room name
			glob.log.error(boost::format("Zone::addRoom(): Cannot add room because duplicate room name for %1% exists in zone %2%") % roomName % mZoneName);
		}
	} else {
		glob.log.error("Zone::addRoom(): Could not load room data");
	}
}

/// the heartbeat function
/** This function passes a heartbeat down to all the loaded rooms. They may have operations that require intermittent
	maintenance or timers and this is how they get it.
*/
void Zone::heartbeat() {
	for(Room::RoomList::iterator pos = mRoomList.begin(); pos != mRoomList.end(); ++pos) {
		pos->second->heartbeat();
	}

	handleWeather();
}

/// subroutine to handle weather
/** This function determines whether or not it's time to shift the zone's weather map
	or to change the direction of the wind
*/
void Zone::handleWeather() {
	// don't forget: some zones don't have weather
	if(!mZoneMap.hasWeather()) {
		return;
	}

	// change the weather if we haven't for a while
	++mTimeSinceLastWindChange;

	if(mTimeSinceLastWindChange > mMinimumStableWindPeriod + (2 * glob.RNG.d100())) {
		glob.log.debug("Zone::handleWeather(): Changing wind direction");
		mZoneMap.changeWind();
		updateRoomWeather();
		mTimeSinceLastWindChange = 0;
	}

	// shift the weather map if it's time
	++mTimeSinceLastWeatherMapShift;
	unsigned int waterMark = 0;

	switch(mZoneMap.getWindStrength()) {
		case 1:
			waterMark = 600;
			break;
		case 2:
			waterMark = 400;
			break;
		case 3:
			waterMark = 300;
			break;
		case 4:
			waterMark = 200;
			break;
		case 5:
			waterMark = 100;
			break;
		default:
			waterMark = 1000;
	}

	if(mTimeSinceLastWeatherMapShift > waterMark) {
		glob.log.debug("Zone::handleWeather(): shifting weather map");
		mTimeSinceLastWeatherMapShift = 0;

		mZoneMap.doWeather();
		updateRoomWeather();
	}
}

/// updates each room with the current weather at its location
/** This function retrieves the weather character for each room's location and sends it to
	the room, which can then process changes and send messages downstream.
*/
void Zone::updateRoomWeather() {
	if(!mZoneMap.hasWeather()) {
		return;
	}

	for(Room::RoomList::iterator it = mRoomList.begin(); it != mRoomList.end(); ++it) {
		char w = mZoneMap.getWeatherChar(it->second->getX(), it->second->getY());

		it->second->setWeather(w, mZoneMap.getWindStrength(), mZoneMap.getWindDirection());
	}
}

/// gets a RoomPointer from the zone
/** This function attempts to find a room with the specified name within the zone and if it does
	returns it to the caller. In the case of a missing room, a blank, instantiated Room object
	is returned instead.
	@param roomName The name of the room to look for
	\return A RoomPointer to the requested room or a blank room pointer.
*/
Room::RoomPointer Zone::getRoom(const std::string &roomName) const {
	std::string target = Utility::toLower(roomName);
	
	Room::RoomList::const_iterator pos;

	if((pos = mRoomList.find(target)) != mRoomList.end()) {
		return pos->second;
	} else {
		return Room::RoomPointer();
	}

}

/// saves all rooms
/** This function is for internal use only and will immediately attempt to save all rooms. Rooms should normally save
	themselves on a timer basis. They're good like that.
	@param force Whether or not to force a save even though no data has changed
*/
void Zone::saveAll(bool force) {
	for(Room::RoomList::iterator pos = mRoomList.begin(); pos != mRoomList.end(); ++pos) {
		if(force) {
			if(!pos->second->Save()) {
				glob.log.error(boost::format("Zone::saveAll(): Not able to save room %1%") % pos->second->getName());
			}
		} else {
			if(pos->second->hasChanged()) {
				if(!pos->second->Save()) {
					glob.log.error(boost::format("Zone::saveAll(): Not able to save room %1%") % pos->second->getName());
				}
			}
		}
	}
}

/// gets a localized map for a specified area
/** This function calls the ZoneMap object's getRadiusMap() function if a map is available and passes it back to
	the caller.
	@param x An x-coordinate
	@param y A y-coordinate
	@param radius Radius of the map to generate
	@param showLegend Whether or not to show a legend with the map
	\return A map to send to the client
*/
std::string Zone::getRadiusMap(const unsigned int x, const unsigned int y, const unsigned int radius, bool showLegend) const {
	std::string map = "Map not available";

	if(mHasMap) {
		map = mZoneMap.getRadiusMap(x, y, radius, showLegend);
	}

	return map;
}

/// checks details of the zone and rooms and makes sure they jive
/** This function does some sanity checks to make sure the zone data integrity is intact.
	Any bad stuff will be logged.
*/
void Zone::consistencyCheck() {
	glob.log.debug(boost::format("Running consistency check for zone %1%") % mZoneName);

	// enforce the requirement that every worldmap position has an associated room.
	if(Utility::iCompare(mZoneMap.getName(), "world")) {
		// there is a map, so make sure it's got a room for each map location
		unsigned int maxX = mZoneMap.getMaxX();
		unsigned int maxY = mZoneMap.getMaxY();

		for(unsigned int x=0; x<maxX; ++x) {
			for(unsigned int y=0; y<maxY; ++y) {
				std::string tempFileName = boost::str(boost::format("%1%x_%2%y") % x % y);
				Room::RoomPointer test = getRoom(tempFileName);

				if(!test) {
					glob.log.debug(boost::format("Need to create a new %1%") % tempFileName);

					// no room with this name currently exists, create on
					Room::RoomPointer room(new Room);

					room->setFileName(tempFileName);
					room->setZoneName(mZoneName);
					room->setName(Utility::toProper(mZoneName));

					room->setX(x);
					room->setY(y);

					ObjectLocation loc;

					loc.type = RoomObject;
					loc.zone = mZoneName;
					loc.location = tempFileName;

					room->setLocation(loc);

					int nx = x;
					int ny = y-1;

					if(ny >= 0) {
						Exit::ExitPointer north(new Exit);

						north->setName("north");
						north->setDestinationZone(mZoneName);
						north->setDestination(boost::str(boost::format("%1%x_%2%y") % nx % ny));

						room->addExit(north);
					}

					int sx = x;
					int sy = y+1;

					if(sy < static_cast<int>(maxY)) {
						Exit::ExitPointer south(new Exit);

						south->setName("south");
						south->setDestinationZone(mZoneName);
						south->setDestination(boost::str(boost::format("%1%x_%2%y") % sx % sy));

						room->addExit(south);
					}

					int wx = x-1;
					int wy = y;

					if(wx >= 0) {
						Exit::ExitPointer west(new Exit);

						west->setName("west");
						west->setDestinationZone(mZoneName);
						west->setDestination(boost::str(boost::format("%1%x_%2%y") % wx % wy));

						room->addExit(west);
					}

					int ex = x+1;
					int ey = y;

					if(ex < static_cast<int>(maxX)) {
						Exit::ExitPointer east(new Exit);

						east->setName("east");
						east->setDestinationZone(mZoneName);
						east->setDestination(boost::str(boost::format("%1%x_%2%y") % ex % ey));

						room->addExit(east);
					}

					int nex = x+1;
					int ney = y-1;

					if((nex < static_cast<int>(maxX)) && (ney >= 0)) {
						Exit::ExitPointer northeast(new Exit);

						northeast->setName("northeast");
						northeast->setDestinationZone(mZoneName);
						northeast->setDestination(boost::str(boost::format("%1%x_%2%y") % nex % ney));

						room->addExit(northeast);
					}

					int nwx = x-1;
					int nwy = y-1;

					if((nwx >= 0) && (nwy >= 0)) {
						Exit::ExitPointer northwest(new Exit);

						northwest->setName("northwest");
						northwest->setDestinationZone(mZoneName);
						northwest->setDestination(boost::str(boost::format("%1%x_%2%y") % nwx % nwy));

						room->addExit(northwest);
					}

					int sex = x+1;
					int sey = y+1;

					if((sex < static_cast<int>(maxX)) && (sey < static_cast<int>(maxY))) {
						Exit::ExitPointer southeast(new Exit);

						southeast->setName("southeast");
						southeast->setDestinationZone(mZoneName);
						southeast->setDestination(boost::str(boost::format("%1%x_%2%y") % sex % sey));

						room->addExit(southeast);
					}

					int swx = x-1;
					int swy = y+1;

					if((swx >= 0) && (swy < static_cast<int>(maxY))) {
						Exit::ExitPointer southwest(new Exit);

						southwest->setName("southwest");
						southwest->setDestinationZone(mZoneName);
						southwest->setDestination(boost::str(boost::format("%1%x_%2%y") % swx % swy));

						room->addExit(southwest);
					}

					if(!room->Save()) {
						glob.log.error(boost::format("Zone::consistencyCheck(): Could not save new room %1% for zone %2%") % tempFileName % mZoneName);
					} else {
						if(!mRoomList.insert(std::make_pair(tempFileName, room)).second) {
							glob.log.error(boost::format("Zone::consistencyCheck(): Error inserting new room %1% into room list for zone %2%. Duplicate name??") % tempFileName % mZoneName);
						}
					}
				}
			}
		}
	}

	glob.log.debug(boost::format("Consistency check for zone %1% is finished") % mZoneName);
}

void Zone::validateExits() {
	/// \todo write validation code
}
