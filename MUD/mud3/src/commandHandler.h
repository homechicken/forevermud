#ifndef MUD_COMMAND_HANDLER_H
#define MUD_COMMAND_HANDLER_H

#include <map>
#include <boost/shared_ptr.hpp>
#include "player.h"
#include "command.h"

/// Processes player input and calls commands
/** This class takes text from connected clients and determines what command to
	call. It splits the command from any additional arguments and checks the
	command to see if it can process with the provided arguments. If not, it calls
	the command's help(). If it can, it then calls the commands process() function.
*/
class CommandHandler {
public:
	/// typedef of the actual map of command names to command pointers
	typedef std::map<std::string, Command *> CommandMap;

	CommandHandler();
	~CommandHandler();

	bool call(Player::PlayerPointer player, const std::string &txt);
	StringVector getCommandList(Player::PlayerPointer player) const;

private:
	void loadCommands();

	CommandMap mCommandList;	///< a std::map of all available commands
};

#endif // MUD_COMMAND_HANDLER_H
