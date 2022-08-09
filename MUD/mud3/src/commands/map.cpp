#include <string>
#include <sstream>

#include "map.h"
#include "utility.h"

#include "global.h"
extern Global glob;


/// Constructor
/** sets the required permission level to execute this command
*/
Map::Map() {
	//mMinimumPermissionLevel = Player::AdminPermissions;
}

/// Destructor
/** Does nothing
*/
Map::~Map() {
}

/// Singleton getter
Map & Map::Instance() {
	static Map instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Map::getName() {
	return "map";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Map::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: map [nokey]~res" << END;
		s << "  ~br0Map~res shows you a map of the area you are in, if available. The ";
		s << "entire world map will not be available, but a small area around you will ";
		s << "be shown instead. For smaller areas, the entire map will be shown. The ";
		s << "optional argument ~b00nokey~res suppresses the display of an explanatory ";
		s << "key.";
	}
	player->Write(s.str());
	player->Prompt();
	return true;
}

/// checks to see if the command works with the arguments provided
/** This command evaluates the arguments and decides whether or not process()
	can be called correctly. If not, the CommandHandler calls the help() function.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command will run properly
*/
bool Map::canProcess(Player::PlayerPointer player, const std::string &txt) {
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Map::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	bool showKey = true;

	if(!txt.empty()) {
		if(Utility::iCompare(txt, "nokey")) {
			showKey = false;
		} else if(Utility::iCompare(txt, "-h")) {
			return help(player);
		}
	}

	ObjectLocation loc = player->getLocation();

	Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);

	if(zone) {
		if(zone->hasMap()) {
			Room::RoomPointer room = zone->getRoom(loc.location);
			if(room) {
				std::string map = zone->getRadiusMap(room->getX(), room->getY(), 3, showKey);

				player->Write(map);
				player->Prompt();
			}
		}
	}

	return true;
}
