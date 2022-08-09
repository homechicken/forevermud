#ifndef MESSAGEDAEMON_H
#define MESSAGEDAEMON_H

#include "mudconfig.h"
#include "message.h"
#include "chatChannel.h"

/// Manages all messages and chat channels
/** This class takes care of all the messaging needs of the server.
	\see ChatChannel
	\see Message
	\todo rewrite ChannelList to use std::map
*/
class MessageDaemon {
public:
	/// definition shortcut for a vector of ChatChannel objects
	typedef std::map<std::string, ChatChannel::ChannelPointer> ChannelMap;

	MessageDaemon();
	~MessageDaemon();

	bool processMessage(Message::MessagePointer message);

	bool addChannel(const std::string &name);
	bool removeChannel(const std::string &name);

	StringVector getChannelList() const;
	StringVector getChannelMembers(const std::string &channel) const;

	bool joinChannel(const std::string &channel, const std::string &name);
	bool leaveChannel(const std::string &channel, const std::string &name);

	void leaveAll(const std::string &name);

private:
	ChannelMap mChannelMap; ///< a map of all active chat channels in the game
	
	bool channelExists(const std::string &name);
};

#endif /* MESSAGEDAEMON_H */
