#ifndef MUD_STATS_H
#define MUD_STATS_H

#include "command.h"
#include "player.h"

/// displays statistical information
/** This class displays driver statistics on read/write, loop processing time, etc.
*/
class Stats : public Command {
public:
	static Stats & Instance();
	virtual ~Stats();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Stats();
	Stats(const Stats &);
	Stats & operator=(const Stats &);
};
#endif // MUD_STATS_H
