#include <string>
#include <sstream>
#include "help.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
Help::Help() {
}

/// Destructor
/** Does nothing
*/
Help::~Help() {
}

/// Singleton getter
Help & Help::Instance() {
	static Help instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Help::getName() {
	return "help";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Help::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: help <category|command>~res" << END;
		s << "  ~br0Help~res gives you information about different categories. Valid categories ";
		s << "are ~b00commands~res. If ~b00help~res is called with a command, that command's " ;
		s << "help information will be returned.";
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
bool Help::canProcess(Player::PlayerPointer player, const std::string &txt) {
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
bool Help::process(Player::PlayerPointer player, const std::string &txt) {
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

	std::stringstream s;

	if(txt == "commands") {
		s << "All commands accept ~b00-h~res as an argument to request help for the ";
		s << "command itself. Arguments between <angle brackets> are ~b00required~res, ";
		s << "and arguments between [square brackets] are optional." << END << END;
		s << "Valid commands are:" << END;
		player->Write(s.str());
		player->Write(glob.commandHandler.getCommandList(player));
		player->Prompt();
		return true;
	}

	// we KNOW txt is not empty, so we extract the first argument in case it was followed by
	// more text
	s << args[0] << " -h";
	glob.commandHandler.call(player, s.str());

	return true;
}
