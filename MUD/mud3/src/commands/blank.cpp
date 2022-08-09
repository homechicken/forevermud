#include <string>
#include <sstream>

#include "REPLACE.h"
/#include "utility.h"

//#include "global.h"
//extern Global glob;


/// Constructor
/** sets the required permission level to execute this command
*/
REPLACE::REPLACE() {
	//mMinimumPermissionLevel = Player::AdminPermissions;
}

/// Destructor
/** Does nothing
*/
REPLACE::~REPLACE() {
}

/// Singleton getter
REPLACE & REPLACE::Instance() {
	static REPLACE instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string REPLACE::getName() {
	return "REPLACE";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool REPLACE::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: REPLACE <required> [optional]~res" << END;
		s << "  ~br0REPLACE~res ...";
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
bool REPLACE::canProcess(Player::PlayerPointer player, const std::string &txt) {

}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool REPLACE::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.empty()) {
		return false;
	}

	StringVector args = Util.stringToVector(txt, " ");
	// since we already checked for txt.empty(), args is always at least size 1

	if(Utility::iCompare(args[0], "-h")) {
		return help(player);
	}

	/*	optionally, use this check instead
	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}
	*/
}
