#ifndef MUD_BAN_H
#define MUD_BAN_H

#include "command.h"
#include "player.h"

/// adds and removes IP addresses from the BanMap
/** This class implements commands to add and remove IP bans on the BanMap object
	of the server.
*/
class Ban : public Command {
public:
	static Ban & Instance();

	virtual ~Ban();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Ban();
	Ban(const Ban &);
	Ban& operator=(const Ban &);

	bool hasValidArguments(const std::string &txt);
	bool extractArguments(const std::string &txt, std::string &command, std::string &ip, std::string &reason);
};
#endif // MUD_BAN_H
