#ifndef MUD_CHANNEL_H
#define MUD_CHANNEL_H

#include "command.h"
#include "player.h"

/// a class to manage chat channel subscriptions
/** This class manages chat channel subscriptions for a player
*/
class Channel : public Command {
public:
	static Channel & Instance();

	virtual ~Channel();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Channel();
	Channel(const Channel &);
	Channel & operator=(const Channel &);
};
#endif // MUD_CHANNEL_H
