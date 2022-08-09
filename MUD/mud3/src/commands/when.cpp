#include <string>
#include <sstream>
#include "when.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
When::When() {
}

/// Destructor
/** Does nothing
*/
When::~When() {
}

/// Singleton getter
When & When::Instance() {
	static When instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string When::getName() {
	return "when";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool When::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: when <player>~res" << END;
		s << "  ~br0When~res tells you how long ago a player logged on.";
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
bool When::canProcess(Player::PlayerPointer player, const std::string &txt) {
	if(txt.empty()) {
		return false;
	} else {
		return true;
	}
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool When::process(Player::PlayerPointer player, const std::string &txt) {
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

	Player::PlayerPointer target = glob.playerDatabase.getPlayer(args[0]);
	if(!target) {
		player->Write("Nobody logged in by that name!");
		player->Prompt();
		return true;
	}
	std::stringstream s;
	s << Utility::toProper(txt) << " has been on for ";
	s << glob.statEngine.getTimeDifference(time(NULL), target->getLogonTime());
	player->Write(s.str());
	player->Prompt();
	return true;
}
