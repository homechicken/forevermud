#include <string>
#include <sstream>
#include <iomanip>

#include "social.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
Social::Social() {
	loadSocials();
}

/// Destructor
/** Does nothing
*/
Social::~Social() {
}

/// Singleton getter
Social & Social::Instance() {
	static Social instance;
	return instance;
}

/// loads socials from a storage device
/** This function requests the \c socials file from the storage layer and loads
	all data into the social list.
*/
void Social::loadSocials() {
	glob.log.debug("Social Command is loading socials...");

	IOResourceLocator resloc;

	resloc.type = DataObject;
	resloc.name = "socials";

	std::string socialFile = glob.ioDaemon.getResource(resloc);

	StringVector socialVector = Utility::stringToVector(socialFile, "#");

	for(StringVector::iterator it = socialVector.begin(); it != socialVector.end(); ++it) {
		SocialData data;

		if(data.loadFromString(*it)) {
			socialMap.insert(std::make_pair(data.getName(), data));
		}
	}
}

/// returns the name of this command
/** This function returns the name of this command only
	\return the name of this command
*/
std::string Social::getName() {
	return "social";
}

/// shows help information
/** This function shows the player help information for this command.
	@param player the player issuing the command
	\return always true
*/
bool Social::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: social <name> [target]~res" << END;
		s << "  ~br0Social~ress are actions you perform on yourself or others. Target ";
		s << "is an optional person upon whom you wish to perform the ~br0social~res. Type ";
		s << "~b00social list~res to see a list of all socials.";
	}
	player->Write(s.str());
	player->Prompt();
	return true;
}

/// decides if the arguments passed allow work with this command
/** This function decides whether the arguments passed to it are correct and allow
	the command to run as expected. We always expect some arguments to this function
	@param player the player issuing the command
	@param txt the arguments to the command
	\return true if the command will run with the specified arguments
*/
bool Social::canProcess(Player::PlayerPointer player, const std::string &txt) {
	if(txt.empty()) {
		return false;
	}
	return true;
}

/// runs the command with the arguments passed
/** This function processes the command with the arguments sent. If a target is given
	(the second argument inside \e txt), the Room the player is in will be searched
	for a matching object. If one is found, and it is a player, a 'target' message will
	be sent to that player. If it is a different type of object, it will not.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command was executed properly
*/
bool Social::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.empty()) {
		return false;
	}

	StringVector args = Utility::stringToVector(txt, " ");
	// since we already checked for txt.empty(), args is always at least size 1

	if(Utility::iCompare(args[0], "-h")) {
		return help(player);
	}

	if(Utility::iCompare(args[0], "list")) {
		std::stringstream s;
		s << "Valid socials are: " << END;
		player->Write(s.str());

		StringVector list;
		for(SocialMap::iterator it = socialMap.begin(); it != socialMap.end(); ++it) {
			list.push_back(it->first);
		}
		player->Write(list);
		player->Prompt();
		return true;
	}

	// locate the social (args[0]) and call it with an optional target (args[1])
	SocialMap::iterator pos = socialMap.find(args[0]);

	if(pos != socialMap.end()) {
		(pos->second).configureFor(player->getSex());

		Message::MessagePointer message = Message::MessagePointer(new Message);
		message->setType(Message::Social);
		message->setFrom(player->getName());

		std::stringstream s;
		std::string myName = Utility::toProper(player->getName());

		if(args.size() == 1) {
			// use the singular form of the emote
			player->Write(pos->second.getSingularSelf());
			player->Prompt();

			// send a copy to the room so others can see
			message->setBody(pos->second.getSingularOthers(myName));
			glob.messageDaemon.processMessage(message);
			return true;
		}

		if(args.size() == 2) {
			// use the plural form of the emote
			std::vector<Physical::PhysicalPointer> targets;

			ObjectLocation loc = player->getLocation();

			if(loc.type == RoomObject) {
				Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);
				if(!zone) {
					glob.log.error(boost::format("Social::process(): Player %1%'s zone %2% cannot be fetched!") % player->getName() % loc.zone);
					return false;
				}

				Room::RoomPointer room = zone->getRoom(loc.location);

				if(room) {
					targets = room->containerFind(args[1]);
				} else {
					glob.log.error(boost::format("Social::process(): Player %1%'s room %2% in zone %3% cannot be fetched!") % player->getName() % loc.location % loc.zone);
					return false;
				}
			}

			if(targets.size() == 0) {
				s << "There is no ~b00" << args[1] << "~res here!";
				player->Write(s.str());
				player->Prompt();

				return true;
			}

			if(targets.size() > 1) {
				// multiple matches for this target!
				s << "There are " << targets.size() << " ~b00" << args[1] << "~res ";
				s << "in this location. I'm assuming you meant the first one.";

				player->Write(s.str());
			}

			std::string targetName = targets[0]->getName();

			// we have exactly one match, this should be the typical case
			Player::PlayerPointer target = glob.playerDatabase.getPlayer(targetName);

			if(target) {
				// the match we have is a player
				targetName = Utility::toProper(targetName);
			}

			player->Write(pos->second.getPluralSelf(targetName));
			player->Prompt();

			// send a copy to the target
			message->setRcpt(targetName);
			message->setBody(pos->second.getPluralTarget(myName, targetName));
			glob.messageDaemon.processMessage(message);

			// send a copy to the room for others to see
			message->setHeader(targetName);
			message->setRcpt("");
			message->setBody(pos->second.getPluralOthers(myName, targetName));
			glob.messageDaemon.processMessage(message);

			return true;
		}

		player->Write("Too many arguments to ~b00social~res!");
		player->Prompt();

		return true;
	} else {	//if(pos != socialMap.end())
		player->Write("No such social. Try ~b00 social list~res for a full list.");
		player->Prompt();

		return true;
	}
}
