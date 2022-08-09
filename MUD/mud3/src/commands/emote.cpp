#include <string>
#include <sstream>

#include "emote.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
Emote::Emote() {
}

/// Destructor
/** Does nothing
*/
Emote::~Emote() {
}

/// Singleton getter
Emote & Emote::Instance() {
	static Emote instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Emote::getName() {
	return "emote";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Emote::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: emote <text>~res" << END;
		s << "  ~br0Emote~res lets you send an emote. You can include the phrase ";
		s << "%ME% to add your own name in the emote.";
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
bool Emote::canProcess(Player::PlayerPointer player, const std::string &txt) {
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
bool Emote::process(Player::PlayerPointer player, const std::string &txt) {
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

	std::string emote = Utility::stringReplace(txt, "%ME%", Utility::toProper(player->getName()));

	Message::MessagePointer message = Message::MessagePointer(new Message);
	message->setType(Message::Emote);
	message->setFrom(player->getName());
	message->setBody(emote);
	glob.messageDaemon.processMessage(message);

	player->Prompt();

	return true;
}
