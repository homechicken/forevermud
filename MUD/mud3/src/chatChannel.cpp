#include <sstream>

#include "utility.h"
#include "chatChannel.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Nothing happens here
*/
ChatChannel::ChatChannel() {
}

/// Destructor
/** Nothing happens here
*/
ChatChannel::~ChatChannel() {
}

/// adds a player to the subscriber list
/** This function lets you add subscribers to the channel list
	@param playername The name of the player to add
	@param isAdmin True if the user has admin rights on the channel, default is \b false
	\return a bool, true if able to add the user to the channel
	\note Will only return false if the player is banned or already on the subscriber list
*/
bool ChatChannel::add(const std::string &playername, bool isAdmin) {
	if(isListener(playername) || isBanned(playername)) {
		return false;
	}

	if(!isBanned(playername)) {
		mListeners.push_back(playername);
		if(isAdmin) {
			mAdmins.push_back(playername);
		}
		return true;
	}
	return false;
}

/// removes a subscriber from the channel
/** This function removes a name from the subscriber list
	@param playername Name of the player to remove
	@param isBanned Whether the removal should also include a future ban
	\return a bool, true if able to remove the user
	\note A false return probably means the user wasn't on the list
*/
bool ChatChannel::remove(const std::string &playername, bool isBanned) {
	if(!isListener(playername)) {
		return false;
	}
	StringVector::iterator pos;
	for(pos = mListeners.begin(); pos != mListeners.end(); ++pos) {
		if(*pos == playername) {
			mListeners.erase(pos);
			if(isBanned) {
				mBanned.push_back(playername);
			}
			return true;
		}
	}
	return false;
}

/// checks to see if the user is in the subscriber list
/** This functions checks the subscriber list for a name matching the one
	provided.
	@param playername The name of the player to check for
	\return a bool, true if the user is a member of the channel
*/
bool ChatChannel::isListener(const std::string &playername) const {
	StringVector::const_iterator pos;
	for(pos = mListeners.begin(); pos != mListeners.end(); ++pos) {
		if(Utility::iCompare(*pos, playername)) {
			return true;
		}
	}
	return false;
}

/// checks to see if the user is banned from the channel
/** This function checks the ban list for a name maching the one provided.
	@param playername The name of the player to check for
	\return a bool, true if the user is banned from this channel
*/
bool ChatChannel::isBanned(const std::string &playername) const {
	StringVector::const_iterator pos;
	for(pos = mBanned.begin(); pos != mBanned.end(); ++pos) {
		if(*pos == playername) {
			return true;
		}
	}
	return false;
}

/// checks to see if the user is an admin for the channel
/** This function checks the admin list for the name matching the one provided
	@param playername The name of the player to check for
	\return a bool, true if the user is an admin for this channel
*/
bool ChatChannel::isAdmin(const std::string &playername) const {
	StringVector::const_iterator pos;
	for(pos = mAdmins.begin(); pos != mAdmins.end(); ++pos) {
		if(*pos == playername) {
			return true;
		}
	}
	return false;
}

/// send a message to everyone in the channel
/** This function sends a copy of the provided message to each of the names
	on the subscriber list, but only if the sender is a member of the channel.
	@param message The message object that should be sent
	\return a bool, true if it was able to send
	\note Message is defined in \b message.h
	\note This function will return false if the sender somehow composes a message
			to the list without being a member.
*/
bool ChatChannel::broadcast(Message::MessagePointer message) {
	// mark the message as having come from the chat channel
	message->setHeader(mName);

	glob.log.debug(boost::format("There are %1% people on channed %2%") % mListeners.size() % mName);

	if(isListener(message->getFrom())) {
		StringVector::iterator pos;
		for(pos = mListeners.begin(); pos != mListeners.end(); ++pos) {
			Player::PlayerPointer player = glob.playerDatabase.getPlayer(*pos);
			if(player) {
				player->processMessage(message);
			}
		}
		return true;
	} else {
		glob.log.info(boost::format("%1% tried to send a message to channel %2%, but wasn't subscribed to it.") % message->getFrom() % mName);
	}
	return false;
}

/// generates a list of all subscribers on the channel
/** This function returns a list of all subscribers listening to this
	channel in proper case form.
	\return a StringVector of all listeners
*/
StringVector ChatChannel::getListeners() const {
	StringVector list;

	for(StringVector::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it) {
		list.push_back(Utility::toProper(*it));
	}
	return list;
}
