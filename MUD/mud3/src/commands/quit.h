#ifndef MUD_QUIT_H
#define MUD_QUIT_H

#include "command.h"
#include "player.h"

/// allows players to leave the game
/** This class lets a player leave the game safely and will save their information
	for them.
*/
class Quit : public Command {
public:
	static Quit & Instance();
	virtual ~Quit();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Quit();
	Quit(const Quit &);
	Quit & operator=(const Quit &);
};
#endif // MUD_QUIT_H
