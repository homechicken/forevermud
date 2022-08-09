#include <string>
#include <sstream>

#include "create.h"
#include "utility.h"
#include "milestone.h"
#include "book.h"
#include "coin.h"

#include "global.h"
extern Global glob;

/// Constructor
/** sets the required permission level to execute this command
*/
Create::Create() {
	mMinimumPermissionLevel = Player::AdminPermissions;
}

/// Destructor
/** Does nothing
*/
Create::~Create() {
}

/// Singleton getter
Create & Create::Instance() {
	static Create instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Create::getName() {
	return "create";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Create::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: create <object>~res" << END;
		s << "  ~br0Create~res lets a player create an object in the virtual world. ";
		s << "The object must be programmed into the ~b00create~b00 command, and the ";
		s << "player must have the proper permissions to do so.";
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
bool Create::canProcess(Player::PlayerPointer player, const std::string &txt) {
	bool allowed = false;

	if(player->getPermissionLevel() >= mMinimumPermissionLevel) {
		allowed = true;
	}

	if(txt.empty()) {
		allowed = false;
	}

	return allowed;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Create::process(Player::PlayerPointer player, const std::string &txt) {
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
		player->Write("You are not in a location that supports creation!");
		player->Prompt();
		return true;
	}

	Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);
	if(!zone) {
		glob.log.error(boost::format("Create::process(): Player %1%'s zone %2% cannot be found") % player->getName() % loc.zone);
		return false;
	}

	Room::RoomPointer room = zone->getRoom(loc.location);
	if(!room) {
		glob.log.error(boost::format("Create::process(): Player %1%'s room %2% in zone %3% cannot be found") % player->getName() % loc.location % loc.zone);
		return false;
	}

	if(Utility::iCompare(txt, "book")) {
		boost::shared_ptr<Book> book(new Book);
		book->setLocation(loc);
		if(!room->containerAdd(book)) {
			glob.log.error("Create::process(): Cannot add book to room");
		} else {
			room->flagChange();
			player->Write("Book created");
		}
	} else if(Utility::iCompare(txt, "coin")) {
		boost::shared_ptr<Coin> coin(new Coin);
		coin->setLocation(loc);
		coin->setQuantity(1);
		coin->setWeight(1);
		coin->setLength(1);
		coin->setWidth(1);
		coin->setHeight(0);
		coin->addAlloy("copper", 90);
		coin->addAlloy("iron", 10);

		if(!room->containerAdd(coin)) {
			glob.log.error("Create::process(): Cannot add coin to room");
		} else {
			room->flagChange();
			player->Write("Coin created");
		}
	} else {
		player->Write("Create a WHAT?");
	}

	player->Prompt();
	return true;
}

