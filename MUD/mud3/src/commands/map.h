#ifndef MUD_MAP_H
#define MUD_MAP_H

#include "command.h"
#include "player.h"

/// shows the player a map of their area
/** This object gets a map of the area around the player from the Zone object
	and sends it to them.
*/
class Map : public Command {
public:
	static Map & Instance();
	virtual ~Map();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Map();
	Map(const Map &);
	Map & operator=(const Map &);
};
#endif // MUD_MAP_H
