#ifndef MUD_SHUTDOWN_H
#define MUD_SHUTDOWN_H

#include "command.h"
#include "player.h"

/// creates an object
/** This class allows a player with the proper permissions to shut down the game
*/
class Shutdown: public Command {
public:
	static Shutdown & Instance();
	virtual ~Shutdown();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Shutdown();
	Shutdown(const Shutdown &);
	Shutdown & operator=(const Shutdown &);
};
#endif // MUD_SHUTDOWN_H
