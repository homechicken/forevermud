#ifndef MUD_TELL_H
#define MUD_TELL_H

#include "command.h"
#include "player.h"

/// sends text to another player
/** This class sends text from one player to another without letting anyone else
	see it.
*/
class Tell : public Command {
public:
	static Tell & Instance();
	virtual ~Tell();
	
	bool help(Player::PlayerPointer player);
	
	virtual std::string getName();
	
	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);
	
private:
	Tell();
	Tell(const Tell &);
	Tell & operator=(const Tell &);
};
#endif // MUD_TELL_H
