#ifndef MUD_WHEN_H
#define MUD_WHEN_H

#include "command.h"
#include "player.h"

/// shows how long a player has been logged in to the game
/** This class gets the login time from the target player and calculates the
	difference between the current time and displays it in a readable string.
*/
class When : public Command {
public:
	static When & Instance();
	virtual ~When();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	When();
	When(const When &);
	When & operator=(const When &);
};
#endif // MUD_WHEN_H
