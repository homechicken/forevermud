#include "connStateClosed.h"

#include "global.h"

extern Global glob;

ConnectionState_Closed::ConnectionState_Closed() {
	glob.log.debug("Created a closed state handler");
}

ConnectionState_Closed::~ConnectionState_Closed() {
	glob.log.debug("Destroyed a closed state handler");
}

void ConnectionState_Closed::process(const std::string &command) {
	glob.log.error("The process() function was called on a closed connection state handler!");
}
