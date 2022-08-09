#include <string>
#include <sstream>

#include "global.h"
extern Global glob;

#include "description.h"
#include "utility.h"

/// Constructor
/** Does nothing
*/
Description::Description() {
}

/// Destructor
/** Does nothing
*/
Description::~Description() {
}

/// Singleton getter
Description & Description::Instance() {
	static Description instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Description::getName() {
	return "Description";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Description::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: description <text>~res" << END;
		s << "  ~br0Description~res lets you write a full description for your character. ";
		s << "You can use as many lines as you like, although writing a novel is not ";
		s << "recommended and may get you banned. " << END;
		s << "  To see your current description, use ~b00look " << player->getName();
		s << "~res" << END;
		s << "  To erase your description and start over, use ~b00description clear~res" << END;
		s << "  Do not use backslashes, none of the characters will be escaped. If you need ";
		s << "a newline character, use ~b00%n~res.";
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
bool Description::canProcess(Player::PlayerPointer player, const std::string &txt) {
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
bool Description::process(Player::PlayerPointer player, const std::string &txt) {
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

	if(Utility::iCompare(args[0], "clear")) {
		player->changeDescription("");
		player->Write("Description cleared");
		player->Prompt();
		return true;
	}
	std::string newDescription = Utility::stringReplace(txt, "\\", "");
	newDescription = Utility::stringReplace(newDescription, "%n", END);

	std::string currentDesc = player->getVerboseDescription();

	std::stringstream s;

	if(currentDesc.empty()) {
		s << newDescription;
	} else {
		s << currentDesc << " " << newDescription;
	}

	player->changeDescription(s.str());
	player->Write("Appended text to description");
	player->Prompt();

	return true;
}
