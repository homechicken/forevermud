#include <string>
#include <sstream>

#include "shutdown.h"

#include "global.h"
extern Global glob;

/// Constructor
/** sets the required permission level to execute this command
*/
Shutdown::Shutdown() {
	mMinimumPermissionLevel = Player::AdminPermissions;
}

/// Destructor
/** Does nothing
*/
Shutdown::~Shutdown() {
}

/// Singleton getter
Shutdown & Shutdown::Instance() {
	static Shutdown instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Shutdown::getName() {
	return "shutdown";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Shutdown::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: shutdown [countdown]~res" << END;
		s << "  ~br0Shutdown~res tells the game engine to finish up its work and exit. ";
		s << "Optionally, an integer countdown can be provided to delay the shutdown by ";
		s << "the specified number of heartbeats.";
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
bool Shutdown::canProcess(Player::PlayerPointer player, const std::string &txt) {
	bool allowed = false;

	if(player->getPermissionLevel() >= mMinimumPermissionLevel) {
		allowed = true;
	}

	return allowed;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Shutdown::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	bool success = false;

	int t = 0;

	if(!txt.empty()) {
		if(txt.substr(0, 2) == "-h") {
			return help(player);
		} else {
			t = Utility::toInt(txt);

			if(t < 1) {
				player->Write("Countdown parameter not understood. Make it an integer");
				player->Prompt();
			} else {
				// schedule a shutdown
				success = true;
				player->Write("The timer feature has not yet been implemented");
				player->Prompt();
			}
		}
	} else {
		// shut down now
		success = true;
		player->Write("Shutting down..");
		glob.shutdownMUD = true;
	}

	return success;
}

