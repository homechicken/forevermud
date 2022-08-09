#include <string>
#include <sstream>
#include <boost/regex.hpp>

#include "ban.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** sets the permission level to Admin
*/
Ban::Ban() {
	mMinimumPermissionLevel = Player::AdminPermissions;
}

/// Destructor
/** Does nothing
*/
Ban::~Ban() {
}

/// Singleton getter
Ban & Ban::Instance() {
	static Ban instance;
	return instance;
}

/// returns the name of this command
/** This function tells you the name of the command
	\return the name of the command
*/
std::string Ban::getName() {
	return "ban";
}

/// returns help info
/** This function tells you how to use the Ban command
	@param player the player sending the command
	\return always true
*/
bool Ban::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: ban <command> [ip] [reason]~res" << END;
		s << "  ~br0Ban~res disallows an IP address from being able to log in to the MUD, showing ";
		s << "them an optional reason (flame, taunt, etc), if provided. Valid commands are: ";
		s << "~b00list~res, ~b00add~res, ~b00remove~res.";
	}
	player->Write(s.str());
	player->Prompt();
	return true;
}

/// checks to see if the command works with the arguments provided
/** This command evaluates the arguments and decides whether or not the command
	can be called correctly. If not, the CommandHandler calls the help() function.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command will run properly
*/
bool Ban::canProcess(Player::PlayerPointer player, const std::string &txt) {
	StringVector args = Utility::stringToVector(txt, " ");
	if(args.empty()) {
		return false;
	}
	if(args[0] == "-h") {
		return true;
	}
	if(args[0] == "list") {
		return true;
	}
	if(args[0] == "remove") {
		if(args.size() == 2) {
			// to remove a ban, we need the "remove" command, and the ip
			return true;
		}
		return false;
	}
	if(args[0] == "add") {
		switch(args.size()) {
			case 2:
			case 3:
				// to add a ban, we need the "add" command, the ip, and accept an optional reason
				return true;
				break;
			default:
				return false;
		}
	}
	return false;
}

/// runs the command
/** This function processes the command with the arguments provided. It only allows
	players with Admin Permissions to add or remove a ban. If no reason is provided
	to ban an IP, a default (\c nobody \c likes \c you) is sent to the BanMap object.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
	\note This function should always end by calling player->Prompt()
*/
bool Ban::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	std::stringstream s;

	StringVector args = Utility::stringToVector(txt, " ");

	if(args.size() > 0) {
		if(args[0] == "-h") {
			return help(player);
		}
		bool result = false;

		if(args[0] == "list") {
			s << "Banned users are: " << END;
			s << glob.banMap.listBannedUsers();
			player->Write(s.str());
			player->Prompt();
			result = true;
		} else if(args[0] == "remove") {
			if(args.size() == 2) {
				// second argument is IP address
				if(glob.banMap.removeBan(args[1])) {
					s << "Ban on " << args[1] << " lifted.";
					player->Write(s.str());
					player->Prompt();
					result = true;
				}
			}
		} else if(args[0] == "add") {
			// decide if we received an IP address as our argument, or a player name
			std::string badIP;

			boost::regex e("^([0-9]{1,3}\\.){3}[0-9]{1,3}$");
			boost::cmatch match;

			if(boost::regex_match(args[1].c_str(), match, e)) {
				badIP = args[1];
			} else {
				// ban by player name
				Player::PlayerPointer p = glob.playerDatabase.getPlayer(args[1]);
				if(p) {
					badIP = p->getHostIP();
				}
			}

			// if something went wrong, fall back on our argument
			if(badIP.empty()) {
				badIP = args[1];
			}

			switch(args.size()) {
				case 2:
					// no reason provided, use the default
					if(glob.banMap.ban(badIP, "nobody likes you")) {
						s << "Banned " << badIP << ", reason: ~b00nobody likes you~res.";
						player->Write(s.str());
						player->Prompt();
					} else {
						// ban had a problem
						player->Write(glob.banMap.getErrorMessage());
						player->Prompt();
					}
					result = true;
					break;

				case 3:
					if(glob.banMap.ban(badIP, args[2])) {
						s << "Banned " << badIP << ", reason: ~b00" << args[2] << "~res.";
						player->Write(s.str());
						player->Prompt();
					} else {
						player->Write(glob.banMap.getErrorMessage());
						player->Prompt();
					}
					result = true;
					break;

				default:
					result = false;
			}
		}
		return result;
	} else {
		// not enough arguments?? we should never get here
		glob.log.error("The ban command was called without any arguments!");
	}
	return false;
}
