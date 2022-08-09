#include <string>
#include <sstream>

#include "global.h"
extern Global glob;

#include "save.h"

/// Constructor
/** sets the required permission level to execute this command
*/
Save::Save() {
}

/// Destructor
/** Does nothing
*/
Save::~Save() {
}

/// Singleton getter
Save & Save::Instance() {
	static Save instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Save::getName() {
	return "save";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Save::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: save~res" << END;
		s << "  ~b00Save~res lets you save your character data at any point in time you ";
		s << "wish, to be restored at a later date. The ForeverMUD engine will periodically save ";
		s << "your character data as well.";
		/// \todo make characters save periodically
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
bool Save::canProcess(Player::PlayerPointer player, const std::string &txt) {
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Save::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	player->Save();
	player->Write("You have been ~b00saved~res. Go forth and sin no more.");
	player->Prompt();
	return true;
}
