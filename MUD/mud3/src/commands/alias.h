#ifndef MUD_ALIAS_H
#define MUD_ALIAS_H

#include "command.h"
#include "player.h"

/// use this command to make aliases for other commands
/** This class lets the user add and remove aliases for commands
*/
class Alias : public Command {
public:
	static Alias & Instance();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

	virtual ~Alias();
private:
	Alias();

	// copy constructor is private
	Alias(const Alias &);
	// so is assignment operator
	Alias& operator=(const Alias &);
};
#endif // MUD_ALIAS_H
