#ifndef MUD_BSOTG_H
#define MUD_BSOTG_H

#include "command.h"
#include "player.h"

/// this is a command to punish dumb players
/** This command is for admin use to punish players. It adds the condition \c bitchslapped
	to the player's condition list.
*/
class Bsotg : public Command {
public:
	static Bsotg & Instance();

	virtual ~Bsotg();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Bsotg();
	Bsotg(const Bsotg &);
	Bsotg & operator=(const Bsotg &);

};
#endif // MUD_BSOTG_H
