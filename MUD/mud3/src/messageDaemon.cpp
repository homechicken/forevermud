#include <iomanip>
#include <sstream>

#include "messageDaemon.h"
#include "player.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** This constructor creates a single channel upon startup, called \c chat
*/
MessageDaemon::MessageDaemon() {
	addChannel("chat");
}
/// Destructor
/** Does nothing
*/
MessageDaemon::~MessageDaemon() {
}

/// reads an incoming message and determines what should be done with it
/** This function takes incoming messages and routes them where ever they need
	to go.
	@param message a copy of the Message object being sent
	\return true if it was able to deliver the message
*/
bool MessageDaemon::processMessage(Message::MessagePointer message) {
	Zone::ZonePointer zone;
	Room::RoomPointer room;
	Player::PlayerPointer player;

	ChannelMap::iterator pos;

	switch(message->getType()) {
	case Message::Chat:
		pos = mChannelMap.find(Utility::toLower(message->getRcpt()));
		
		if(pos != mChannelMap.end()) {
			return pos->second->broadcast(message);
		} else {
			glob.log.error("MessageDaemon::processMessage(): A chat message came in for a nonexistant chat channel");
			return false;
		}
		
		break;

	case Message::Tell:
		if(message->getFrom().empty() || message->getRcpt().empty()) {
			// we don't receive tells from nobody
			return false;
		}
		
		player = glob.playerDatabase.getPlayer(message->getRcpt());
		
		if(player) {
			player->processMessage(message);
			return true;
		} else {
			// player not logged in?
			Message::MessagePointer msg = Message::MessagePointer(new Message);;
			msg->setType(Message::Information);
			msg->setFrom("Message Daemon");
			msg->setRcpt(message->getFrom());
			msg->setBody("No such player logged in.");
			processMessage(msg);

			return false;
		}
		
		break;

	case Message::Quit:
		glob.playerDatabase.broadcast(message);
		return true;
		break;

	case Message::Speech:	// these types all fall through to default processing
	case Message::Emote:
	case Message::Social:
	case Message::Information:
	default:
		if(message->getRcpt().empty()) {
			// no specific recipient means it goes to the room the sender is in
			player = glob.playerDatabase.getPlayer(message->getFrom());
			if(player) {
				ObjectLocation loc = player->getLocation();
				if(loc.type == RoomObject) {
					zone = glob.zoneDaemon.getZone(loc.zone);

					if(zone) {
						room = zone->getRoom(loc.location);

						if(room) {
							glob.log.debug("Sending Speech|Emote|Social|Info to Room");
							room->processMessage(message);
							return true;
						}
					}
				}
			}
		} else {
			// assume recipient is a player and send it
			/// \todo handle mobs here as well?
			player = glob.playerDatabase.getPlayer(message->getRcpt());
			if(player) {
				player->processMessage(message);
			}
			return true;
		}

		glob.log.debug("Message of type Speech|Emote|Social|Information failed delivery");
		return false;
	}

	return false;
}

/// adds a public channel
/** This function creates a new public channel
	@param name the name of the channel to create
	\return true if a new channel was created with the given name
*/
bool MessageDaemon::addChannel(const std::string &name) {
	if(channelExists(name)) {
		return false;
	}

	ChatChannel::ChannelPointer c = ChatChannel::ChannelPointer(new ChatChannel);
	mChannelMap[Utility::toLower(name)] = c;
	
	return true;
}

/// checks for the existence of a channel by name
/** This function searches the local map for a channel with the name provided 
	@param name The name of a channel for which to look
	\return true if a channel of that name exists
*/
bool MessageDaemon::channelExists(const std::string &name) {
	ChannelMap::iterator pos = mChannelMap.find(Utility::toLower(name));
	return pos != mChannelMap.end();
}

/// removes a channel from the list
/** This function deletes a channel from the list
	@param name the name of the channel to delete
	\return true if the channel was in the list and is now deleted
*/
bool MessageDaemon::removeChannel(const std::string &name) {
	ChannelMap::iterator pos = mChannelMap.find(Utility::toLower(name));
	
	if(pos != mChannelMap.end()) {
		mChannelMap.erase(pos);
		return true;
	} else {
		return false;
	}
}

/// generates a list of all channels
/** This function makes a list of all channels
	\return a StringVector of all chat channel names
	\todo pass a PlayerPointer here and check permissions on each channel?
*/
StringVector MessageDaemon::getChannelList() const {
	StringVector list;

	for(ChannelMap::const_iterator pos = mChannelMap.begin(); pos != mChannelMap.end(); ++pos) {
		// we use the internal channel name just in case it's prettier than the toLower() version keyed by the map
		list.push_back(pos->second->getName());
	}

	return list;
}

/// gets a list of subscribers to the channel
/** This function gets a list of listeners from a channel.
	@param channel the name of the channel
	\return a StringVector of all channel listeners or a blank StringVector
*/
StringVector MessageDaemon::getChannelMembers(const std::string &channel) const {
	StringVector list;

	ChannelMap::const_iterator pos = mChannelMap.find(Utility::toLower(channel));
	
	if(pos != mChannelMap.end()) {
		list = pos->second->getListeners();
	}

	return list;
}

/// adds a listener to a channel
/** This function adds a listener to a channel.
	@param channel the name of the channel
	@param name the name of the listener
	\return true if the ChatChannel allows the user in, false if they're banned or the channel is gone
*/
bool MessageDaemon::joinChannel(const std::string &channel, const std::string &name) {
	bool res = false;
	
	ChannelMap::iterator pos = mChannelMap.find(Utility::toLower(channel));
	
	if(pos != mChannelMap.end()) {
		res = pos->second->add(name);
	}

	return res;
}

/// removes a listener from a channel
/** This function removes a listener from the ChatChannel
	@param channel the name of the channel
	@param name the name of the listener
	\return true if the listener was removed
*/
bool MessageDaemon::leaveChannel(const std::string &channel, const std::string &name) {
	bool res = false;
	
	ChannelMap::iterator pos = mChannelMap.find(Utility::toLower(channel));
	
	if(pos != mChannelMap.end()) {
		res = pos->second->remove(name);
	}
	
	return res;
}

/// removes a listener from all channels
/** This function, called during client logout, removes the client from all channels
	so messages won't be sent to the client anymore.
	@param name the name of the client
*/
void MessageDaemon::leaveAll(const std::string &name) {
	ChannelMap::iterator pos;
	
	for(pos = mChannelMap.begin(); pos != mChannelMap.end(); ++pos) {
		pos->second->remove(name);
	}
}

