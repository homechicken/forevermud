#ifndef MUD_WHO_H
#define MUD_WHO_H

#include "command.h"
#include "player.h"

/// a class to tell who's logged in
/** This class creates a list of logged-in players. If the player has Admin
	permissions, the reverse DNS resolved hostname is displayed next to the
	player's name.
*/
class Who : public Command {
public:
	static Who & Instance();
	virtual ~Who();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Who();
	Who(const Who &);
	Who & operator=(const Who &);
};
#endif // MUD_WHO_H
