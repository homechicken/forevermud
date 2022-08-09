#ifndef MUD_SOCIAL_H
#define MUD_SOCIAL_H

#include <map>
#include "command.h"
#include "player.h"
#include "socialData.h"

/// a class to manage social interactions
/** This class controls social interactions such as \c wave , \c bow , \c nod ,
	etc.
*/
class Social : public Command {
public:
	typedef std::map<std::string, SocialData> SocialMap;

	static Social & Instance();

	virtual ~Social();
	
	bool help(Player::PlayerPointer player);
	
	virtual std::string getName();
	
	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);

private:
	Social();
	Social(const Social &);
	Social & operator=(const Social &);

	void loadSocials();

	SocialMap socialMap;
};
#endif // MUD_SOCIAL_H
