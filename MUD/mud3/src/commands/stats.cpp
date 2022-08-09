#include <string>
#include <sstream>

#include "stats.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Does nothing
	\todo make this admin only?
*/
Stats::Stats() {
}

/// Destructor
/** Does nothing
*/
Stats::~Stats() {
}

/// Singleton getter
Stats & Stats::Instance() {
	static Stats instance;
	return instance;
}

/// tells you the name of this command
/** This function tells you the name of this command
	\return the name of this command
*/
std::string Stats::getName() {
	return "stats";
}

/// returns help info
/** This function explains how to use this command
	@param player the player sending the command
	\return always true
*/
bool Stats::help(Player::PlayerPointer player) {
	std::stringstream s;
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		s << glob.Config.getStringValue("AdminRequired");
	} else {
		s << "~br0Usage: stats~res" << END;
		s << "  ~br0Stats~res displays statistics about the ForeverMUD engine.";
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
bool Stats::canProcess(Player::PlayerPointer player, const std::string &txt) {
	return true;
}

/// runs the command
/** This function processes the command with the arguments provided.
	@param player the player sending the command
	@param txt the arguments to the command
	\return true if the command executed properly
*/
bool Stats::process(Player::PlayerPointer player, const std::string &txt) {
	if(player->getPermissionLevel() < mMinimumPermissionLevel) {
		player->Write(glob.Config.getStringValue("AdminRequired"));
		player->Prompt();
		return true;
	}

	if(txt.length() > 1 && txt.substr(0,2) == "-h") {
		return help(player);
	}

	time_t seconds = glob.statEngine.getEngineUptimeSeconds();

	unsigned long bytesIn = glob.statEngine.getBytesIn();
	unsigned long bytesOut = glob.statEngine.getBytesOut();

	std::stringstream s;
	s << "Writing " << bytesOut / seconds << " bytes per second." << END;
	s << "Reading " << bytesIn / seconds << " bytes per second." << END;
	s << "Average loop processing time is " << glob.statEngine.getAverageLoopProcessTime() << " microseconds." << END;
	s << "There are " << glob.zoneDaemon.getNumberOfZones() << " zones loaded." << END;
	s << "There are " << glob.zoneDaemon.getTotalNumberOfRooms() << " rooms in all the zones.";

	player->Write(s.str());
	player->Prompt();
	return true;
}
