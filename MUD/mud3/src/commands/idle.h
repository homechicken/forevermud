#ifndef MUD_IDLE_H
#define MUD_IDLE_H

#include "command.h"

/// tells you how long a player has been idle
/** This class figures out how long a player has been idle and returns it
*/
class Idle : public Command {
public:
	static Idle & Instance();
	virtual ~Idle();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Idle();
	Idle(const Idle &);
	Idle & operator=(const Idle &);
};
#endif // MUD_IDLE_H
