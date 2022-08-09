#include <string>
#include <sstream>

#include "alias.h"
#include "utility.h"

/// Constructor
/** Does nothing
*/
Alias::Alias() {
}

/// Destructor
/** Does nothing
*/
Alias::~Alias() {
}

/// Singleton instatiator/get function
/** This function ensures we only ever have one copy of this class in memory and provides
	a method to get said copy
*/
Alias & Alias::Instance() {
	static Alias instance;
	return instance;
}

/// returns help info
/** This function tells you how to use the Alias command
	@param player the player sending the command
	\return always true
*/
bool Alias::help(Player::PlayerPointer player) {
	std::stringstream s;
	s << "~br0Usage: alias <name> <command>~res" << END;
	s << "  ~br0Alias~res lets you create a shortcut for a longer command, for example: ";
	s << "~b00alias l look~res would create an alias for ~b00l~res that expands to ~b00look~res ";
	s << "when the command is parsed. An ~br0alias~res is ~u00only~res expanded if it appears at ";
	s << "the beginning of the command, and will ~u00not~res be parsed anywhere else." << END;
	s << "  Use ~b00alias list~res to see a list of your current aliases.";
	player->Write(s.str());
	player->Prompt();
	return true;
}

/// gets the name of this command
/** This function returns the name of this command
	\return the name of this command
*/
std::string Alias::getName() {
	return "alias";
}

/// checks to see if the command works with the arguments provided
/** This command evaluates the arguments and decides whether or not the command
	can be called correctly. If not, the CommandHandler calls the help() function.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command will run properly
*/
bool Alias::canProcess(Player::PlayerPointer player, const std::string &txt) {
	if(txt.empty()) {
		return false;
	}
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
	\note This function should always end by calling player->Prompt()
*/
bool Alias::process(Player::PlayerPointer player, const std::string &txt) {
	std::string expanded = "";
	std::string alias = Utility::stringGetFirst(txt, " ", expanded);

	if(alias.empty()) {
		return help(player);
	}

	if(Utility::iCompare(alias, "list")) {
		// show a list of current aliases
		player->Write(player->getAliasList());
		player->Prompt();
		return true;
	}

	if(expanded.empty()) {
		// if there is no \c expanded here, we have a problem
		return help(player);
	}

	player->addAlias(alias, expanded);
	player->Write("Alias added.");
	player->Prompt();

	return true;
}
