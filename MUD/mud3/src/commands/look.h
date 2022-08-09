#ifndef MUD_LOOK_H
#define MUD_LOOK_H

#include "command.h"
#include "player.h"

/// gets object descriptions
/** This class allows a player to examine an object and returns the object's verbose
	description.
*/
class Look: public Command {
public:
	static Look & Instance();
	virtual ~Look();
	
	bool help(Player::PlayerPointer player);
	
	virtual std::string getName();
	
	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);
	
private:
	Look();
	Look(const Look &);
	Look & operator=(const Look &);
};
#endif // MUD_LOOK_H
