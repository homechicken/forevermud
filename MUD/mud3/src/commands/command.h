#ifndef MUD_COMMAND_H
#define MUD_COMMAND_H

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "mudconfig.h"
#include "player.h"

/// Pure virtual class to derive game commands from
/** This class should be the base to derive all game commands from. The 
	CommandHandler will call canProcess() first, if it returns false then the
	CommandHandler will call help(), otherwise it calls process().
*/
class Command {
public:
	Command();
	virtual ~Command(){};
	
	/// gets the name of the command, pure virtual and must be overridden by inheritor
	virtual std::string getName() = 0;
	
	/// gets the help string, pure virtual and must be overridden by the inheritor
	virtual bool help(Player::PlayerPointer player) = 0;
	
	/// Determines whether the input is enough to run this command
	/** This function tells the CommandHandler that calls it whether it can
		process this command with the provided argument (txt).
	*/
	virtual bool canProcess(Player::PlayerPointer player, const std::string &txt) = 0;
	
	/// Actually runs the command
	/** \note ALL output from this function and its inheritors should end with a call
		to Player->Prompt() at the bare minimum!
	*/
	virtual bool process(Player::PlayerPointer player, const std::string &txt) = 0;
	
	/// returns the minimum permission level required to execute this command
	Player::PermissionLevel getMinimumPermissionLevel() { return mMinimumPermissionLevel; }

protected:
	Player::PermissionLevel mMinimumPermissionLevel; ///< Permission level required to execute

private:
	
};

#endif // MUD_COMMAND_H
