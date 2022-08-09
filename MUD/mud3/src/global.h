// global.h
#ifndef MUD_GLOBAL_H
#define MUD_GLOBAL_H

#include "socketDriver.h"
#include "statEngine.h"
#include "banMap.h"
#include "playerDatabase.h"
#include "commandHandler.h"
#include "messageDaemon.h"
#include "eventDaemon.h"
#include "io.h"
#include "log.h"
#include "objectFactory.h"
#include "random.h"
#include "zoneDaemon.h"
#include "runtimeConfig.h"

/// Holds all global data
/** This class manages all the global data used by the game.
*/
class Global {
public:
	Global();
	~Global();

	IO ioDaemon;					///< handles file i/o
	Log log;						///< does all logging
	RuntimeConfig Config;			///< manages the run-time configuration
	PlayerDatabase playerDatabase;	///< keeps track of players
	StatEngine statEngine;			///< keeps track of engine statistics
	BanMap banMap;					///< stores info about banned players and IP addresses
	SocketDriver driver;			///< the main communication object
	CommandHandler commandHandler;	///< looks up and processes commands
	MessageDaemon messageDaemon;	///< handles sending messages
	EventDaemon eventDaemon;		///< keeps track of timed events
	ObjectFactory Factory;			///< creates new objects
	Random RNG;						///< generates random numbers
	ZoneDaemon zoneDaemon;			///< holds all zone information

	bool shutdownMUD;	///< Set to true when it's time to shut down
	bool saveRooms;		///< Set to true when it's time to autosave all rooms and their items

private:


};

#endif // MUD_GLOBAL_H
