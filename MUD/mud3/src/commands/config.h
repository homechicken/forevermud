#ifndef MUD_CONFIG_H
#define MUD_CONFIG_H

#include <vector>
#include "command.h"
#include "player.h"

/// sets player-based configuration values
/** This class lets a player control many default values related to their connection,
	such as screen width, whether they see ANSI color in their terminal, what their
	prompt should say, etc.
*/
class Config : public Command {
public:
	static Config & Instance();

	virtual ~Config();

	bool help(Player::PlayerPointer player);

	virtual std::string getName();

	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Config();
	Config(const Config &);
	Config & operator=(const Config &);

	std::string getOption(const std::string &txt);
	std::string getSetting(const std::string &txt);
};
#endif // MUD_CONFIG_H
