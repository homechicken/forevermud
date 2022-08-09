#include <string>
#include <sstream>

#include "idle.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** sets the required permission level to execute this command
*/
Idle::Idle() {
}

/// Destructor
/** Does nothing
*/
Idle::~Idle() {
}

/// Singleton getter
Idle & Idle::Instance() {
	static Idle instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Idle::getName() {
	return "idle";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Idle::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: idle <player>~res" << END;
		s << "  ~br0Idle~res shows how much time has passed since a logged-in player ";
		s << "has issued a command.";
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
bool Idle::canProcess(Player::PlayerPointer player, const std::string &txt) {
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
bool Idle::process(Player::PlayerPointer player, const std::string &txt) {
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
		player->Write("No such client logged in!");
		player->Prompt();
		return true;
	}
	std::stringstream s;
	s << Utility::toProper(args[0]) << " has been idle for ";
	s << glob.statEngine.getTimeDifference(time(NULL), target->getLastCommandTime());

	player->Write(s.str());
	player->Prompt();
	return true;
}
