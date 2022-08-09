#include "zoneDaemon.h"
#include "zone.h"

#include "global.h"
extern Global glob;

/// constructor
/** The constructor loads all the zones in the zone directory and sets
	the autosave timer.
*/
ZoneDaemon::ZoneDaemon() {
	loadAllZones();

	int autosaveTimer = glob.Config.getIntValue("AutosaveTimer");

	if(autosaveTimer < 1) {
		autosaveTimer = 300;
	}

	mHeartbeatsToNextSave = autosaveTimer;
}

/// destructor
/** The destructor saves all the zones before it exits
*/
ZoneDaemon::~ZoneDaemon() {
	saveAllZones();
}

/// checks settings regularly
/** This function is called on every heartbeat (default 3 seconds). It passes the heartbeat
	call to each zone, and checks to see if the timer is done and should trigger an autosave.
*/
void ZoneDaemon::heartbeat() {
	for(Zone::ZoneList::iterator it = mZoneList.begin(); it != mZoneList.end(); ++it) {
		it->second->heartbeat();
	}

	if(mHeartbeatsToNextSave == 0) {
		glob.saveRooms = true;

		int autosaveTimer = glob.Config.getIntValue("AutosaveTimer");

		if(autosaveTimer < 1) {
			autosaveTimer = 300;
		}

		mHeartbeatsToNextSave = autosaveTimer;
	}

	--mHeartbeatsToNextSave;
}

/// gets a zone by name
/** This function looks for a zone with the specified name, and if found, returns
	a pointer to the zone.
	@param zoneName the name of the zone to look for
	\return a pointer to the zone, or a NULL pointer
*/
Zone::ZonePointer ZoneDaemon::getZone(const std::string &zoneName) {
	Zone::ZoneList::iterator it;

	if((it = mZoneList.find(zoneName)) != mZoneList.end()) {
		return it->second;
	} else {
		return Zone::ZonePointer();
	}
}

/// loads all zones
/** This function loads all zones in the zone directory.
*/
void ZoneDaemon::loadAllZones() {
	StringVector allZones = glob.ioDaemon.getZoneList();

	if(allZones.size() < 1) {
		glob.log.error("ZoneDaemon::loadAllZones(): There are no zones to load! Something is really going to break in a second...");
		return;
	}

	for(StringVector::iterator it = allZones.begin(); it != allZones.end(); ++it) {
		Zone::ZonePointer zone = Zone::ZonePointer(new Zone);

		zone->setName(*it);

		glob.log.debug(boost::format("ZoneDaemon::loadAllZones(): Loading zone %1% data...") % *it);

		zone->load();

		if(!mZoneList.insert(std::make_pair(*it, zone)).second) {
			glob.log.error(boost::format("ZoneDaemon::loadAllZones(): This should never happen, but there seems to be a duplicate zone name %1%") % *it);
			continue;
		}
	}
	glob.log.debug("ZoneDaemon is done loading zones");
}

/// validates all exits in a zone
/** This function checks all exits in a zone and makes sure they point to a real location. Exits cannot
	be validated until \b all zones have loaded (because they may point to rooms outside their own zone).
*/
void ZoneDaemon::validateAllExits() {
	glob.log.debug("ZoneDaemon is validating all zone exits");

	for(Zone::ZoneList::iterator it = mZoneList.begin(); it != mZoneList.end(); ++it) {
		it->second->validateExits();
	}
	glob.log.debug("ZoneDaemon is done validating exits");
}

/// saves all zones
/** This function loops through all zones and tells them to save their rooms.
*/
void ZoneDaemon::saveAllZones() {
	for(Zone::ZoneList::iterator it = mZoneList.begin(); it != mZoneList.end(); ++it) {
		it->second->saveAll();
	}
}

/// shows how many rooms are in the game
/** This function gets a count of the number of rooms for each zone and adds them together,
	returning the total to the caller.
	\return the number of rooms in all the zones
*/
unsigned int ZoneDaemon::getTotalNumberOfRooms() {
	unsigned int numRooms = 0;

	for(Zone::ZoneList::iterator it = mZoneList.begin(); it != mZoneList.end(); ++it) {
		numRooms += it->second->getNumberOfRooms();
	}
	return numRooms;
}

