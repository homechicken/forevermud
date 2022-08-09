#ifndef MUD_DESCRIPTION_H
#define MUD_DESCRIPTION_H

#include "command.h"
#include "player.h"

/// a class to set the verbose description of a player
/** This class allows a player to set his/her verbose description, one line at
	a time.
*/
class Description : public Command {
public:
	static Description & Instance();
	virtual ~Description();
	
	bool help(Player::PlayerPointer player);
	
	virtual std::string getName();
	
	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);
	
private:
	Description();
	Description(const Description &);
	Description & operator=(const Description &);
	
};
#endif // MUD_DESCRIPTION_H
