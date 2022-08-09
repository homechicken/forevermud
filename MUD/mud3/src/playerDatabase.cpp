#include <iomanip>
//#include <algorithm>

#include "playerDatabase.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** The constructor only sets the highest file descriptor to 0
*/
PlayerDatabase::PlayerDatabase() {
	mHighestFd = 0;
}
/// Destructor
/** Does nothing
*/
PlayerDatabase::~PlayerDatabase() {
}

/// Adds a new player to the game.
/** This function adds new Players to the game. It also keeps a look out for socket
	descriptors that are greater than the highest descriptor on file, and sets the
	highest fd appropriately. Looping up to the highest fd is much faster than looping
	through all possible fd's (usually determined by getdtablesize(), the maximum
	number of descriptors a single process can have open).
	@param player a shared_ptr copy of the player object
*/
void PlayerDatabase::add(Player::PlayerPointer player) {
	if(player) {
		glob.log.debug("Calling PlayerDatabase::add with a new player");

		if(player->getFd() > mHighestFd) {
			mHighestFd = player->getFd();
			glob.log.debug(boost::format("PlayerDatabase::add(): Set highest fd to %1%") % mHighestFd);
		}

		mPlayerList.push_back(player);
	} else {
		glob.log.error("PlayerDatabase::add() called with NULL player pointer");
	}
}

/// removes a player from the game by name
/** This function removes a player object from the game by the player's name
	@param name the name of the player to disconnect
	\return true if the name corresponded to a player and was disconnected
*/
bool PlayerDatabase::remove(const std::string &name) {
	bool success = false;

	Player::PlayerPointer player = getPlayer(name);

	if(player) {
		success = remove(player);
	} else {
		glob.log.error(boost::format("PlayerDatabase::remove(name): Failed to get a PlayerPointer for %1%") % name);
	}

	return success;
}

/// removes a player from the game by shared_ptr
/** This function removes a connected player from the game by shared_ptr
	@param player the shared_ptr that points to the Player to remove
	\return true if the shared_ptr was in the list of players
*/
bool PlayerDatabase::remove(Player::PlayerPointer player) {
	bool success = false;

	if(!player) {
		glob.log.error("PlayerDatabase::remove(PlayerPointer) tried to remove a NULL player pointer");
		return success;
	}

	ObjectLocation loc = player->getLocation();

	if(loc.type == RoomObject) {
		Zone::ZonePointer zone = glob.zoneDaemon.getZone(loc.zone);

		if(!zone) {
			glob.log.error(boost::format("PlayerDatabase::remove(): Cannot find player %1%'s zone %2%") % player->getName() % loc.zone);
		} else {
			Room::RoomPointer room = zone->getRoom(loc.location);

			if(!room) {
				glob.log.error(boost::format("PlayerDatabase;:remove(): Cannot find player %1%'s room %2% in zone %3%") % player->getName() % loc.location % loc.zone);
			} else {
				glob.log.debug("Removing player from room");
				room->containerRemove(player);
				success = true;
			}
		}
	}

	int playerFd = player->getFd();

	mPlayerList.erase(std::remove(mPlayerList.begin(), mPlayerList.end(), player), mPlayerList.end());

	glob.driver.shutdown_connection(playerFd);

	return success;
}

/// closes all connections to the server
/** This function is called when the SocketDriver is destructing to make sure that any 
	open sockets have been removed.
*/
void PlayerDatabase::closeAllConnections() {
	if(mPlayerList.size() > 0) {
		mPlayerList.clear();
		glob.log.debug("PlayerDatabase::closeAllConnections() had players in it. They were removed.");
	}
}

/// Gets a Player object by its socket descriptor
/** This function returns a shared_ptr to the player that owns the file descriptor
	if the descriptor matches that of an already logged-in player.
	@param fd the descriptor to look for
	\return a shared_ptr copy of the Player object or a NULL
*/
Player::PlayerPointer PlayerDatabase::getPlayer(const int fd) const {
	PlayerList::const_iterator pos;
	Player::PlayerPointer player;

	for(pos = mPlayerList.begin(); pos != mPlayerList.end(); ++pos) {
		if((*pos)->getFd() == fd) {
			player = *pos;
		}
	}

	return player;
}

/// Gets a Player pointer by name
/** This function returns a shared_ptr to the Player object associated with the
	name provided
	@param name the name to look for
	\return a shared_ptr of the corresponding Player object
*/
Player::PlayerPointer PlayerDatabase::getPlayer(const std::string &name) const {
	PlayerList::const_iterator pos;
	Player::PlayerPointer player;

	for(pos = mPlayerList.begin(); pos != mPlayerList.end(); ++pos) {
		if(Utility::iCompare((*pos)->getName(), name)) {
			player = *pos;
		}
	}

	return player;
}

/// Sends all players a message
/** This function sends a message to every player object that is logged in.
	@param message a Message object that should be sent
	\note this is good for login/logout messages and global notifications, but should
		be used sparingly otherwise.
	\warning This function calls Player::processMessage() directly, and bypasses the
		rebroadcast system built in to all objects that normally receive messages.
*/
void PlayerDatabase::broadcast(Message::MessagePointer message) {
	if(message) {
		PlayerList::iterator pos;

		for(pos = mPlayerList.begin(); pos != mPlayerList.end(); ++pos) {
			(*pos)->processMessage(message);
		}
	} else {
		glob.log.error("PlayerDatabase::broadcast: Received a NULL message");
	}
}

/// delivers a message to a player
/** This function delivers a message to the player specified as the recipient
	@param message A Message shared pointer to deliver
*/
void PlayerDatabase::deliverMessage(Message::MessagePointer message) {
	if(message) {
		Player::PlayerPointer player = getPlayer(message->getRcpt());

		if(player) {
			player->processMessage(message);
		}
	}
}

/// generates a list of logged-in players
/** This function generates a formatted list of current players along with their
	resolved hostnames, if applicable.
	@param includeHost whether or not to include DNS hostnames with each player
	\return a StringMap containing the formatted player list
*/
StringVector PlayerDatabase::getPlayerList(bool includeHost) const {
	StringVector list;

	for(PlayerList::const_iterator it = mPlayerList.begin(); it != mPlayerList.end(); ++it) {
		std::stringstream s;

		s << Utility::toProper((*it)->getName());

		if(includeHost) {
			std::string host = (*it)->getHostname();

			if(host == "Private") {
				host = (*it)->getHostIP();
			}

			s << "  (" << host << ")";
		}

		list.push_back(s.str());
	}

	return list;
}

/// calls all player heartbeat() functions
/** This function makes sure it calls all heartbeat() functions for connected players.
*/
void PlayerDatabase::callHeartbeats() {
	for(PlayerList::iterator it = mPlayerList.begin(); it != mPlayerList.end(); ++it) {
		if((*it)->inPlayState()) {
			(*it)->heartbeat();
		}
	}
}

/// processes waiting commands for all players
/** This function calls the process() func for each player that has a waiting command 
*/
void PlayerDatabase::processCommands() {
	for(PlayerList::iterator it = mPlayerList.begin(); it != mPlayerList.end(); ++it) {
		std::string command = (*it)->getNextCommand();
		if(!command.empty()) {
			(*it)->process(command);
		}
	}
}

