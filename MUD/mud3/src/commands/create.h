#ifndef MUD_CREATE_H
#define MUD_CREATE_H

#include "command.h"
#include "player.h"

/// creates an object
/** This class allows a player with the proper permissions to create a new object
	in the virtual world.
*/
class Create: public Command {
public:
	static Create & Instance();
	virtual ~Create();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Create();
	Create(const Create &);
	Create & operator=(const Create &);
};
#endif // MUD_CREATE_H
