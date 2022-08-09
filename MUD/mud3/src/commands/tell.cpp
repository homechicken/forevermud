#include <string>
#include <sstream>

#include "tell.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
Tell::Tell() {
}

/// Destructor
/** Does nothing
*/
Tell::~Tell() {
}

/// Singleton getter
Tell & Tell::Instance() {
	static Tell instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Tell::getName() {
	return "tell";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Tell::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: tell <player> <message>~res" << std::endl;
		s << "~br0Tell~res lets you send a message directly to another logged-in " << std::endl;
		s << "player." << std::endl;
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
bool Tell::canProcess(Player::PlayerPointer player, const std::string &txt) {
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
bool Tell::process(Player::PlayerPointer player, const std::string &txt) {
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

	if(Utility::iCompare(args[0], "-h") || args.size() < 2) {
		return help(player);
	}

	if(Utility::iCompare(args[0], player->getName())) {
		player->Write("Talking to yourself again, eh?");
		player->Prompt();
		return true;
	}

	Message::MessagePointer message = Message::MessagePointer(new Message);
	message->setType(Message::Tell);
	message->setFrom(player->getName());
	message->setRcpt(args[0]);
	message->setBody(txt.substr(args[0].length() + 1));

	if(glob.messageDaemon.processMessage(message)) {
		player->Prompt();
		return true;
	} else {
		player->Write("Could not send message!");
		player->Prompt();
		return false;
	}
}
