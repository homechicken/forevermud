#ifndef ZONE_DAEMON
#define ZONE_DAEMON

#include "zone.h"

/// manages all the zones in the game
/** This class organizes the zones for the game.
*/
class ZoneDaemon {
public:
	ZoneDaemon();
	~ZoneDaemon();

	void heartbeat();

	Zone::ZonePointer getZone(const std::string &zoneName);

	unsigned int getNumberOfZones()	{ return mZoneList.size(); }

	unsigned int getTotalNumberOfRooms();

	void saveAllZones();

private:
	Zone::ZoneList mZoneList;	///< holds all the zone objects

	void loadAllZones();

	void validateAllExits();

	int mHeartbeatsToNextSave;	///< how long until the autosave goes off?

};

#endif // ZONE_DAEMON
