#ifndef MUD_EMOTE_H
#define MUD_EMOTE_H

#include "command.h"
#include "player.h"

/// a class to allow players to interact with others in ways other than speech
/** This class handles player emotes, a way of expressing yourself that differs
	from normal speech.
	\see Social
	\see SocialData
*/
class Emote : public Command {
public:
	static Emote & Instance();
	virtual ~Emote();
	
	bool help(Player::PlayerPointer player);
	
	virtual std::string getName();
	
	bool canProcess(Player::PlayerPointer player, const std::string &txt);
	bool process(Player::PlayerPointer player, const std::string &txt);
	
private:
	Emote();
	Emote(const Emote &);
	Emote & operator=(const Emote &);
};
#endif // MUD_EMOTE_H
