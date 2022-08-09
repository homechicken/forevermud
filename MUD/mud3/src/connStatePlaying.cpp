#include "connStatePlaying.h"

#include "global.h"
extern Global glob;

ConnectionState_Playing::ConnectionState_Playing() {
	glob.log.debug("Created a playing state connection handler");
}

ConnectionState_Playing::~ConnectionState_Playing() {
	glob.log.debug("Destroyed a playing state connection handler");
}

void ConnectionState_Playing::process(const std::string &command) {
	mPlayer->resetLastCommandTime();
	glob.commandHandler.call(mPlayer, command);
}
