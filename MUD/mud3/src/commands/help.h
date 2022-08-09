#ifndef MUD_HELP_H
#define MUD_HELP_H

#include "command.h"
#include "player.h"

/// a class to explain things to people
/** This class explains some basic mechanics to new players and will call help on
	any other commands in the game.
	*/
class Help : public Command {
public:
	static Help & Instance();
	virtual ~Help();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Help();
	Help(const Help &);
	Help & operator=(const Help &);
};
#endif // MUD_HELP_H
