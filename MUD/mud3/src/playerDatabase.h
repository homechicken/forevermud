#ifndef MUD_PLAYER_DATABASE_H
#define MUD_PLAYER_DATABASE_H

#include <boost/shared_ptr.hpp>

#include "player.h"
#include "message.h"

/// stores all connected players
/** This class keeps track of all connected players
	\see Player
*/
class PlayerDatabase {
public:
	/// typedef for simplifying writing iterators, and for readability
	typedef std::vector<Player::PlayerPointer> PlayerList;

	PlayerDatabase();
	~PlayerDatabase();

	void add(Player::PlayerPointer player);

	bool remove(const std::string &name);
	bool remove(Player::PlayerPointer player);

	void closeAllConnections();

	/// gets the highest file descriptor that has been connected
	/** This function returns the highest file descriptor that has been connected
		to the system.
		\warning I've tracked down several bugs that I accidentally wrote because of this,
		make sure when you call for highestFd that you ADD ONE to it before you
		loop through all fd's when polling sockets, etc. You can also check for
		equality: for(int i=0; i<=getHighestFd(); ++i), just don't do i<getHighestFd()!
	*/
	int getHighestFd() const { return mHighestFd; }

	/// returns the number of currently connected players
	unsigned int getNumberOfConnections() const { return mPlayerList.size(); }

	Player::PlayerPointer getPlayer(const int fd) const;
	Player::PlayerPointer getPlayer(const std::string &name) const;

	void broadcast(Message::MessagePointer message);

	void deliverMessage(Message::MessagePointer message);

	StringVector getPlayerList(bool includeHost = false) const;

	void callHeartbeats();
	
	void processCommands();

private:
	PlayerList mPlayerList;	///< a list of currently connected players
	
	int mHighestFd;	///< the highest file descriptor that has been seen so far
};

#endif // MUD_PLAYER_DATABASE_H
