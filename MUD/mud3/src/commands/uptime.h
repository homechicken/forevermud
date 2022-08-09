#ifndef MUD_UPTIME_H
#define MUD_UPTIME_H

#include "command.h"
#include "player.h"

/// shows how long the game driver has been up and running
/** This class calculates the difference between the game start time and the current
	time and returns it in a nice, readable string to the client.
*/
class Uptime : public Command {
public:
	static Uptime & Instance();
	virtual ~Uptime();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Uptime();
	Uptime(const Uptime &);
	Uptime & operator=(const Uptime &);
};
#endif // MUD_UPTIME_H
