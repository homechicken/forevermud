#ifndef MUD_TEST_H
#define MUD_TEST_H

#include "command.h"
#include "player.h"

/// sends test output to the client
/** This class sends a test string out to the client. It includes some width text
	as well as ANSI colors and text decorations.
*/
class Test : public Command {
public:
	static Test & Instance();
	virtual ~Test();
	
	bool help(Player::PlayerPointer player);
	
	virtual std::string getName();
	
	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);
	
private:
	Test();
	Test(const Test &);
	Test & operator=(const Test &);
};
#endif // MUD_TEST_H
