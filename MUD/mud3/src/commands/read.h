#ifndef MUD_READ_H
#define MUD_READ_H

#include "command.h"
#include "player.h"

/// lets a person read something written down
/** This class contains functionality to allow players to read things that
	are written down, whether they are in books, on scrolls, carved in stone, etc.
*/
class Read : public Command {
public:
	static Read & Instance();
	virtual ~Read();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Read();
	Read(const Read &);
	Read & operator=(const Read &);
};
#endif // MUD_READ_H
