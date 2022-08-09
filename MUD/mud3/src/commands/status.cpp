#include <string>
#include <sstream>

#include "global.h"
extern Global glob;

#include "status.h"
#include "utility.h"

/// Constructor
/** Does nothing
*/
Status::Status() {
}

/// Destructor
/** Does nothing
*/
Status::~Status() {
}

/// Singleton getter
Status & Status::Instance() {
	static Status instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Status::getName() {
	return "status";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Status::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: status~res" << END;
		s << "  ~br0Status~res tells you about your current state.";
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
bool Status::canProcess(Player::PlayerPointer player, const std::string &txt) {
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Status::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	player->Write(player->getStatusString());
	player->Prompt();
	return true;
}
