#include <sstream>
#include "mudconfig.h"

#include "quit.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
Quit::Quit() {
}

/// Destructor
/** Does nothing
*/
Quit::~Quit() {
}

/// Singleton getter
Quit & Quit::Instance() {
	static Quit instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Quit::getName() {
	return std::string("quit");
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Quit::help(Player::PlayerPointer player) {
	std::stringstream s;
	s << "~br0Usage: quit [exit-message]~res" << END;
	s << "  ~br0Quit~res Initiates the process for a player to leave the game. ";
	s << "An optional string will be broadcast to other players as an exit message. ";
	s <<"For example: ~b00quit later everyone~res will broadcast ~nc0<yourname> logs out: ";
	s <<" later everyone~res.";

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
bool Quit::canProcess(Player::PlayerPointer player, const std::string &txt) {
	// no special arguments to test
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Quit::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	std::stringstream s;
	s << Utility::toProper(player->getName()) << " logs out";

	if(!txt.empty()) {
		s << ": " << txt;
	}

	std::string logout = glob.Config.getStringValue("LogoutMessage");

	if(logout.empty()) {
		logout = "Goodbye";
	}

	player->Write(logout);

	Message::MessagePointer message = Message::MessagePointer(new Message);
	message->setType(Message::Quit);
	message->setFrom(player->getName());
	message->setBody(s.str());

	glob.playerDatabase.broadcast(message);

	glob.playerDatabase.remove(player->getName());

	return true;
}
