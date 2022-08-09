#ifndef ...
#define ...

#include "command.h"
#include "player.h"

/// ...
/** ...
*/
class REPLACE : public Command {
public:
	static REPLACE & Instance();
	
	virtual ~REPLACE();
	
	bool help(Player::PlayerPointer player);
	
	virtual std::string getName();
	
	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);
	
private:
	REPLACE();
	REPLACE(const REPLACE &);
	REPLACE & operator=(const REPLACE &);
};
#endif // ...
