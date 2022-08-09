#ifndef MUD_SAY_H
#define MUD_SAY_H

#include "command.h"
#include "player.h"

/// sends speech to the local environment
/** This command allows a player to speak in a Room.
*/
class Say : public Command {
public:
	static Say & Instance();
	virtual ~Say();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Say();
	Say(const Say &);
	Say & operator=(const Say &);
};
#endif // MUD_SAY_H
