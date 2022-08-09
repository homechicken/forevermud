#ifndef MUD_CHATCHANNEL_H
#define MUD_CHATCHANNEL_H

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include "mudconfig.h"
#include "message.h"

/// A class that stores information about a chat channel
/** This class manages a single chat channel.
	\see MessageDaemon
*/
class ChatChannel {
public:
	typedef boost::shared_ptr<ChatChannel> ChannelPointer;
	
	ChatChannel();
	~ChatChannel();

	/// sets the name of the channel
	std::string getName() const { return mName; }

	/// gets the name of the channel
	void setName(const std::string &name) { mName = name; }

	bool add(const std::string &playername, bool isAdmin = false);
	bool remove(const std::string &playername, bool isBanned = false);

	bool isListener(const std::string &playername) const;
	bool isBanned(const std::string &playername) const;
	bool isAdmin(const std::string &playername) const;

	bool broadcast(Message::MessagePointer message);

	StringVector getListeners() const;

private:
	std::string mName;	///< the name of the chat channel
	StringVector mAdmins;	///< list of admins
	StringVector mBanned;	///< list of banned users
	StringVector mListeners;	///< list of current subscribers
};

#endif /* MUD_CHATCHANNEL_H */
