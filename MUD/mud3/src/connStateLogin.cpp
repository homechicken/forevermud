#include "connStateLogin.h"

#include "global.h"
#include "utility.h"

extern Global glob;

ConnectionState_Login::ConnectionState_Login() {
	glob.log.debug("Created a login state handler");
	mWaiting = false;
	mCreate = false;
}

ConnectionState_Login::~ConnectionState_Login() {
	glob.log.debug("Destroying a login state handler");
}

void ConnectionState_Login::process(const std::string &command) {
	if(!mPlayer) {
		glob.log.error("Login State has no valid PlayerPointer");
		return;
	}

	// we're already waiting for a response here
	if(mWaiting) {
		std::string resp = Utility::toLower(command);
		if(resp == "y" || resp == "yes") {
			// enter create phase
			mPlayer->setConnectionState(Connection::ConnState_Create);
			mPlayer->Write("Password: ");
		} else {
			std::string loginPrompt = glob.Config.getStringValue("LoginPrompt");

			if(loginPrompt.empty()) {
				loginPrompt = "Login: ";
			}

			mPlayer->Write(loginPrompt);
			mWaiting = false;
		}
		return;
	}
	
	int maxNameLen = glob.Config.getIntValue("ShortestAllowedNameLength");
	
	if(command.size() < Utility::intToStdStringSizeT(maxNameLen)) {
		mPlayer->Write("That name is too short, try again!");
		return;
	}

	Player::PlayerPointer p = glob.playerDatabase.getPlayer(command);

	// first, check to see if that player is already logged in
	if(p) {
		mPlayer->Write(boost::format("%1% is already logged in") % Utility::toProper(command));
		glob.log.warn(boost::format("%1% is trying to log in as %2%, who is already logged in from %3") % mPlayer->getHostIP() % command % p->getHostIP());
		glob.playerDatabase.remove(mPlayer);
		return;
	}

	mPlayer->setName(Utility::toLower(command));

	if(!mPlayer->Load()) {
		mPlayer->Write(boost::format("There is no record of a ~b00%1%~res here. Do you wish to create %1%? (y/n): ") % command);
		mWaiting = true;
	} else {
		mPlayer->Write("Password: ");
		mPlayer->setConnectionState(Connection::ConnState_Password);
	}
}
