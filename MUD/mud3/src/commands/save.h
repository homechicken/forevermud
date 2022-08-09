#ifndef MUD_SAVE_H
#define MUD_SAVE_H

#include "command.h"
#include "player.h"

/// saves player data
/** This class saves player data to the configured storage system.
*/
class Save : public Command {
public:
	static Save & Instance();
	virtual ~Save();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Save();
	Save(const Save &);
	Save & operator=(const Save &);
};
#endif // MUD_SAVE_H
