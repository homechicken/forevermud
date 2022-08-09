#include <string>
#include <sstream>

#include "look.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** sets the required permission level to execute this command
*/
Look::Look() {
}

/// Destructor
/** Does nothing
*/
Look::~Look() {
}

/// Singleton getter
Look & Look::Instance() {
	static Look instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Look::getName() {
	return "look";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Look::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: look [object]~res" << END;
		s << "  ~br0Look~res gives you information about your current environment ";
		s << "or optionally about a specific item or player. Some players don't like ";
		s << "it when you ~b00look~res at them, though, so consider yourself warned...";
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
bool Look::canProcess(Player::PlayerPointer player, const std::string &txt) {
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Look::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	ObjectLocation loc = player->getLocation();

	if(loc.type != RoomObject) {
		player->Write("You are not in a room!");
		player->Prompt();
		return true;
	}

	Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);
	if(!zone) {
		glob.log.error(boost::format("Look::process(): Player %1%'s zone %2% cannot be found") % player->getName() % loc.zone);
		return false;
	}

	Room::RoomPointer room = zone->getRoom(loc.location);
	if(!room) {
		glob.log.error(boost::format("Look::process(): Player %1%'s room %2% in zone %3% cannot be found") % player->getName() % loc.location % loc.zone);
		return false;
	}

	if(txt.empty()) {
		// looking at the local environment
		std::stringstream s;

		s << room->getFullDescription(player->getName());

		if(zone->hasWeather()) {
			s << END;
			s << zone->getWeatherForRoom(room->getX(), room->getY());
			s << " " << zone->getWindString();
		}

		player->Write(s.str());
		player->Prompt();

		return true;
	} else {
		// check for an item of interest in the room
		std::string description = room->getItemOfInterest(txt);
		if(!description.empty()) {
			player->Write(description);
			player->Prompt();
			return true;
		}

		// go through all items in the room looking for a match
		std::vector<Physical::PhysicalPointer> inv;
		std::vector<Physical::PhysicalPointer>::iterator invIt;
		Player::PlayerPointer target;

		unsigned int i = 0;
		std::stringstream s;

		inv = room->containerFind(txt);
		switch(inv.size()) {
		case 0:
			s << "I don't know anything about the ~b00" << txt << "~res here";
			break;
		case 1:
			// check to see if we're looking at another player
			target = glob.playerDatabase.getPlayer(inv[0]->getName());
			if(target && target != player) {
				// notify the target they just got ogled :)
				s << Utility::toProper(player->getName()) << " looks you over.";
				target->Write(s.str());
				target->Prompt();
				s.str("");
			}
			s << (inv[0])->getVerboseDescription();
			break;
		default:
			// many matches here
			s << "There are several ~b00" << txt << "~res here: " << END;
			for(invIt = inv.begin(); invIt != inv.end(); ++invIt, ++i) {
				s << i << ": " << (*invIt)->getBriefDescription() << END;
			}
		}

		player->Write(s.str());
		player->Prompt();

		return true;
	}
}
