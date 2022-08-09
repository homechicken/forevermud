#include <string>
#include <sstream>

#include "read.h"
#include "readable.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
Read::Read() {
}

/// Destructor
/** Does nothing
*/
Read::~Read() {
}

/// Singleton getter
Read & Read::Instance() {
	static Read instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Read::getName() {
	return "read";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Read::help(Player::PlayerPointer player) {
	std::stringstream s;
	s << "~br0Usage: read <item> [page]~res" << END;
	s << "  ~br0Read~res lets you read the text written on an item. Optionally," << END;
	s << "you can specify a page number (if there are multiple pages), otherwise" << END;
	s << "you get the first page.";
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
bool Read::canProcess(Player::PlayerPointer player, const std::string &txt) {
	if(txt.empty()) {
		return false;
	}
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
	\todo disambiguate "read book 2" from "read book 2 2" (book 2 page 2)
*/
bool Read::process(Player::PlayerPointer player, const std::string &txt) {
	if(txt.empty()) {
		return false;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	std::stringstream s;
	std::string objectName = "";

	ObjectLocation loc = player->getLocation();

	if(loc.type != RoomObject) {
		glob.log.error(boost::format("Read::process(): Player %1% is not in a room!") % player->getName());
		return false;
	}

	Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);

	if(!zone) {
		glob.log.error(boost::format("Read::process(): Player %1%'s zone %2% could not be found") % player->getName() % loc.zone);
		return false;
	}

	Room::RoomPointer room = zone->getRoom(loc.location);

	if(!room) {
		glob.log.error(boost::format("Read::process(): Player %1%'s room %2% in zone %3% cannot be found") % player->getName() % loc.location % loc.zone);
		return false;
	}

	unsigned int page = 1;

	StringVector args = Utility::stringToVector(txt, " ");
	switch(args.size()) {
	case 1: // we only have an item to read
		objectName = txt; // same as args[0]
		break;
	case 2: // we have our object and a page number
		objectName = args[0];
		page = Utility::toUInt(args[1]);
		if(page == 0) {
			// there was an error casting string to UInt perhaps?
			page = 1;
		}
		break;
	default:
		// assemble middle arguments into ONE object name
		s << "Read command from " << player->getName() << " came with more than one target object argument";
		glob.log.info(s.str());
		s.str("");
		for(unsigned int i = 1; i < args.size() - 2; ++i) {
			s << args[i] << " ";
		}
		objectName = s.str();
		s.str("");
		page = Utility::toUInt(args[args.size() - 1]);
		if(page == 0) {
			page = 1;
		}
	}

	Readable::ReadablePointer ptr;
	std::vector<unsigned int> results;
	std::vector<Physical::PhysicalPointer> inventory, playerInventory;
	std::vector<Physical::PhysicalPointer>::iterator it;
	unsigned int i = 0;

	inventory = room->containerFind(objectName, false);
	playerInventory = player->containerFind(objectName, false);

	/// \todo can't you just add vectors?
	for(it = playerInventory.begin(); it != playerInventory.end(); ++it) {
		inventory.push_back((*it));
	}

	switch(inventory.size()) {
	case 0:
		s << "There is no ~b00" << objectName << "~res here!";
		break;

	case 1:
		ptr = boost::dynamic_pointer_cast<Readable>(inventory[0]);
		if(ptr) {
			glob.log.debug(boost::format("Read::process(): Requesting page %1% for reading") % page);

			unsigned int numPages = ptr->getNumberOfPages();

			if(page > numPages) {
				page = numPages;
			}

			s << ptr->getPage(page);

			if(numPages > 1) {
				s << END << "Page " << page << " of " << numPages;
			}
		}
		break;

	default:
		s << "There is more than one ~b00" << objectName << "~res here:" << END;
		for(it = inventory.begin(); it != inventory.end(); ++it, ++i) {
			s << i << ": " << (*it)->getBriefDescription() << END;
		}
	}

	player->Write(s.str());
	player->Prompt();

	return true;
}
