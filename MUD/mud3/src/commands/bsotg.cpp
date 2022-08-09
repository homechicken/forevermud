#include <string>
#include <sstream>

#include "bsotg.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Sets default permissions to admin level
*/
Bsotg::Bsotg() {
	mMinimumPermissionLevel = Player::AdminPermissions;
}

/// Destructor
/** does nothing
*/
Bsotg::~Bsotg() {
}

/// Singleton getter
Bsotg & Bsotg::Instance() {
	static Bsotg instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Bsotg::getName() {
	return "bsotg";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Bsotg::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: bsotg <player>~res" << END;
		s << "  Use ~br0bsotg~res to administer the Bitchslap Of The Gods to a player ";
		s << "that really has it coming. Like anyone that bothers you.";
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
bool Bsotg::canProcess(Player::PlayerPointer player, const std::string &txt) {
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
*/
bool Bsotg::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.empty()) {
		return false;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	Player::PlayerPointer target = glob.playerDatabase.getPlayer(txt);
	if(!target) {
		player->Write("No such player logged in!");
		player->Prompt();
		return true;
	}

	std::stringstream s;
	s << Utility::toProper(player->getName()) << " administers the Bitchslap of the Gods to you!";
	target->Write(s.str());
	target->Prompt();
	target->addCondition("bitchslapped");

	Event event;
	event.setEventName("bsotg");
	event.setTimeLeft(5);
	event.setTarget(txt);
	event.setTargetObjectType(target->getObjectType());
	glob.eventDaemon.addEvent(event);

	// technically, you can bitchslap yourself, so don't double-prompt if that's the case
	if(player != target) {
		player->Prompt();
	}

	return true;
}
