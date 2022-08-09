#include <string>
#include <sstream>

#include "channel.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
*/
Channel::Channel() {
}

/// Destructor
/** Does nothing
*/
Channel::~Channel() {
}

/// Singleton getter
Channel & Channel::Instance() {
	static Channel instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Channel::getName() {
	return "channel";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Channel::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: channel <command> [channel name|message]~res" << END;
		s << "  ~br0Channel~res gives you access to the chat system. Valid commands are:" << END;
		s << "    ~b00list~res Lists all chat channels" << END;
		s << "    ~b00who <channel>~res Lists all members of ~b00channel~res" << END;
		s << "    ~b00join <channel>~res Subscribe to ~b00channel~res" << END;
		s << "    ~b00leave <channel>~res Unsubscribe to ~b00channel~res" << END;
		s << "    ~b00ban <channel> <name>~res Bans ~b00name~res from subscribing to ~b00channel~res" << END;
		s << "    ~b00<channel> <message>~res Sends ~b00message~res to all subscribers of ~b00channel~res";
	}
	player->Write(s.str());
	player->Prompt();
	return true;
}

/// checks to see if the command works with the arguments provided
/** This command evaluates the arguments and decides whether or not process()
	can be called correctly. If not, the CommandHandler calls the help() function.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command will run properly
*/
bool Channel::canProcess(Player::PlayerPointer player, const std::string &txt) {
	if(txt.empty()) {
		return false;
	}

	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Channel::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.empty()) {
		return false;
	}

	StringVector args = Utility::stringToVector(txt, " ");
	// since we already checked for txt.empty(), args is always at least size 1

	if(Utility::iCompare(args[0], "-h")) {
		return help(player);
	}

	if(Utility::iCompare(args[0], "list")) {
		player->Write(glob.messageDaemon.getChannelList());
		player->Prompt();
		return true;
	}

	if(args.size() < 2) {
		// incorrect arguments from here on out
		return help(player);
	}

	if(Utility::iCompare(args[0], "who")) {
		// args[1] is the channel name here
		player->Write(glob.messageDaemon.getChannelMembers(args[1]));
		player->Prompt();
		return true;
	}

	std::stringstream s;

	if(Utility::iCompare(args[0], "join")) {
		if(glob.messageDaemon.joinChannel(args[1], player->getName())) {
			s << "You have joined ~nc0" << args[1] << "~res";
			player->Write(s.str());
		} else {
			s << "Failed to join ~nc0" << args[1] << "~res channel";
			player->Write(s.str());
			s.str("");
			s << "Player " << player->getName() << " failed to join channel ";
			s << args[1];
			glob.log.info(s.str());
		}
		player->Prompt();
		return true;
	}

	if(Utility::iCompare(args[0], "leave")) {
		if(glob.messageDaemon.leaveChannel(args[1], player->getName())) {
			s << "You have left ~nc0" << args[1] << "~res";
			player->Write(s.str());
		} else {
			s << "Failed to leave ~nc0" << args[1] << "~res";
			player->Write(s.str());
			s.str("");
			s << "Player " << player->getName() << " couldn't leave channel ";
			s << args[1] << "(not a member?)";
			glob.log.info(s.str());
		}
		player->Prompt();
		return true;
	}

	if(Utility::iCompare(args[0], "ban")) {
		/// \todo implement chat banning
		player->Write("Ban is not yet implemented");
		player->Prompt();
		return true;
	}

	Message::MessagePointer message = Message::MessagePointer(new Message);
	message->setType(Message::Chat);
	message->setFrom(player->getName());
	message->setRcpt(args[0]); // the name of the channel
	// if we skip args[0].length() characters into the \c txt argument, that gives
	// us the original message the player wants to send to the channel. We add one
	// to bypass the space after the channel name.
	message->setBody(txt.substr(args[0].length() + 1));

	if(!glob.messageDaemon.processMessage(message)) {
		s << "Message not sent. Either the channel does not exist or you are not a ";
		s << "member of that channel.";
		player->Write(s.str());
		player->Prompt();
	}
	// we don't prompt here because the player will receive a copy of the message,
	// and will get Prompt()-ed again
	return true;
}
