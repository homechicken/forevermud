#ifndef MUD_STATUS
#define MUD_STATUS

#include "command.h"
#include "player.h"

/// a class to show a player's status
/** This command displays many internal variables that describe the player's current
	state.
*/
class Status : public Command {
public:
	static Status & Instance();
	virtual ~Status();
	
	bool help(Player::PlayerPointer player);
	
	virtual std::string getName();
	
	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);
	
private:
	Status();
	Status(const Status &);
	Status & operator=(const Status &);
};
#endif // MUD_STATUS
