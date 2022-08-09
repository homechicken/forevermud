#include "commandHandler.h"
#include "mudconfig.h"
#include "command.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** The constructor loads a copy of all command objects into memory
	\note the function is implemented separately in loadCommands.cpp
*/
CommandHandler::CommandHandler() {
	loadCommands();
}

/// Destructor
/** Doesn't do anything
*/
CommandHandler::~CommandHandler() {
}

/// Takes player input and determines what command to call
/** This function evaluates a command (txt) from a connected player and splits
	any additional arguments from the command, then looks it up in the list. It
	calls the command's canProcess() function which determines whether it can process
	the command with the specified arguments. If it can't, it calls the command's
	help() function. If it can, then the process() function is called.

	@param player A copy of the player's object
	@param txt The command the player object sent to the driver
	\return true if able to call a command
*/
bool CommandHandler::call(Player::PlayerPointer player, const std::string &txt) {
	std::stringstream s;
	std::string arguments;
	std::string command = Utility::stringGetFirst(txt, " ", arguments);

	std::string alias = player->getAlias(command);

	if(!alias.empty()) {
		glob.log.debug(boost::format("Alias found for %1%: '%2%'") % command % alias);
		s << alias;
		if(!arguments.empty()) {
			s << " " << arguments;
		}
		command = Utility::stringGetFirst(s.str(), " ", arguments);

		glob.log.debug(boost::format("Set command to: '%1%' with arguments '%2%'") % command % arguments);
	}

	CommandMap::iterator pos = mCommandList.find(Utility::toLower(command));

	bool result = false;

	// if we find a matching command, call it and return
	if(pos != mCommandList.end()) {
		if(pos->second->canProcess(player, arguments)) {
			result = pos->second->process(player, arguments);
		} else {
			result = pos->second->help(player);
		}
		return result;
	}

	// we didn't find a matching command, check for item- or special exit-names
	ObjectLocation loc = player->getLocation();

	if(loc.type == RoomObject) {
		// player is inside a room
		Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);

		if(!zone) {
			glob.log.error(boost::format("CommandHandler::call(): Cannot fetch player %1%'s zone %2%") % player->getName() % loc.zone);
			return false;
		}

		Room::RoomPointer room = zone->getRoom(loc.location);

		if(!room) {
			glob.log.error(boost::format("CommandHandler::call(): Cannot fetch player %1%'s room %2% in zone %3%") % player->getName() % loc.location % loc.zone);
			return false;
		}


		// ItemsOfInterest matching ala Richard Bartle: http://www.mud.co.uk/richard/vv.htm
		if(command[command.length() - 1] == '?') {
			command = command.substr(0, command.length() - 1);
			std::string itemdesc = room->getItemOfInterest(command);
			if(!itemdesc.empty()) {
				player->Write(itemdesc);
				player->Prompt();
				return true;
			} else {
				player->Write(boost::format("I don't know anything about the ~b00%1%~res here") % command);
				player->Prompt();
				return true;
			}
		}

		// check for special exits
		// We could check this with getExit(command), but that could be a problem.
		// If they really want to exit, only the exit name should have been
		// received as a command, nothing extra on the end.
		Exit::ExitPointer exit = room->getExit(command);
		if(exit) {
			// found a matching exit name, move the player if possible
			if(exit->canPass()) {
				Zone::ZonePointer destinationZone = glob.zoneDaemon.getZone(exit->getDestinationZone());
				if(!destinationZone) {
					glob.log.error(boost::format("CommandHandler::call(): Can't get exit %1%'s destination zone %2%") % exit->getName() % exit->getDestinationZone());
					return true;
				}

				Room::RoomPointer destination = destinationZone->getRoom(exit->getDestination());

				if(!destination) {
					glob.log.error(boost::format("commandHandler::call(): Room %1% in zone %2% can't find destination for exit %3% room %4% in zone %5%!") % room->getFileName() % room->getZoneName() % exit->getName() % exit->getDestination() % exit->getDestinationZone());
					return true;
				}

				Message::MessagePointer message(new Message);

				message->setType(Message::EntranceExit);
				message->setFrom(player->getName());
				message->setBody(boost::format("%1% leaves %2%") % player->getName() % command);

				room->containerRemove(player);
				room->resendMessage(message);
				room->flagChange();

				message->setBody(boost::format("%1% enters the room") % player->getName());

				destination->containerAdd(player);
				destination->resendMessage(message);
				destination->flagChange();

				loc.location = destination->getFileName();
				loc.zone = destination->getZoneName();
				player->setLocation(loc);

				player->Write(destination->getBriefDescription());
				player->Prompt();

				return true;
			} else {
				if(exit->hasDoor() && exit->isClosed()) {
					if(exit->isLocked()) {
						player->Write("The door is locked!");
						player->Prompt();
					} else {
						player->Write("The door is closed!");
						player->Prompt();
					}
				} else {
					glob.log.error(boost::format("Can't figure out why %1%'s exit, %2%, isn't working") % room->getName() % exit->getName());
					player->Write("WTF? Exit code FAIL");
					player->Prompt();
				}
				return true;
			}
		} // if(exit)
	} else { // if(loc.type == RoomObject)
		glob.log.debug("Location is not a RoomObject!");
	}

	// no commands, no items, no exits
	std::string badCommand = glob.Config.getStringValue("BadCommandMessage");

	if(badCommand.empty()) {
		badCommand = "Bad command/exit";
	}

	player->Write(badCommand);
	player->Prompt();

	return false;
}

/// generates a StringVector of all commands in the game
/** This function generates a StringVector of all commands that the player has
	available (aka permissions to execute).
	@param player a shared_ptr to the Player object asking for the command list
	\return a StringVector of all available commands
*/
StringVector CommandHandler::getCommandList(Player::PlayerPointer player) const {
	StringVector list;

	for(CommandMap::const_iterator pos = mCommandList.begin(); pos != mCommandList.end(); ++pos) {
		if(player->getPermissionLevel() >= pos->second->getMinimumPermissionLevel()) {
			list.push_back(pos->first);
		}
	}

	return list;
}
