#include <pthread.h>
#include <semaphore.h>
#include <iostream>

#include "mudconfig.h"
#include "thread_functions.h"

#include "global.h"
Global glob;

/// This is where everything starts
/** This function ignores any arguments and starts the game up.
	It created the driver and processing threads and does nothing else.
	@param argc the number of command-line arguments
	@param argv a \c char array of command-line arguments
	\note There are no command-line arguments for this program
	\todo find a way to let either thread join and shut the other one down
*/
int main(int argc, char *argv[]) {
	if(!USE_MYSQL && USE_MYSQL_LOGGING) {
		std::cerr << "You must enable USE_MYSQL in mudconfig.h before enabling USE_MYSQL_LOGGING\n";
		return MYSQL_ERROR;
	}

	glob.log.debug("Starting ForeverMUD...");

	pthread_attr_t attr;
	pthread_t tProcess;
	pthread_t tSaveRooms;

	glob.driver.initialize(MUDPORT);

	// initialize and start our threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&tProcess, NULL, &thread_process_func, NULL);
	pthread_create(&tSaveRooms, NULL, &thread_saveRooms_func, NULL);

	// if this function returns, the game is shutting down
	thread_driver_func();

	glob.log.debug("ForeverMUD is shutting down.");
	
	pthread_cancel(tSaveRooms);
	pthread_cancel(tProcess);
	
	return 0;
}

/** \mainpage ForeverMUD
	\section intro_sec Introduction
	ForeverMUD is a modern, object-oriented C++ MUD written to use many newer programming practices.

	\section features_sec Features
	- Makes gratuitous use of smart pointers, the standard template library, Boost libraries, and even a design pattern or two
	- Uses boost::spirit to parse grammar on a per-command basis, which allows for extremely complex, tailored grammar rules where needed
	- Cool temperature equalization system
	- All text is formatted on-the-fly, tailored to the client's resolution settings
	- Good code documentation (you're reading it!)
	- The engine is a collection of top-level (global) daemons that manage their own sections of the game
	- "Items Of Interest" matching ala Richard Bartle: http://www.mud.co.uk/richard/vv.htm
	- Uses YAML (YAML Ain't Markup Language) for loading and saving data (http://www.yaml.org/)
	- Multiple threads that handle different functions (sockets, saving, processing commands)
	- Modular, directory-based zone system supporting colored ASCII maps and shifting weather

	\section history_sec History
	This project has gone by many names, and for quite a long time it was only known as "MUD3", because
	it was the third complete rewrite of the project. In June 2010 I settled on the name "ForeverMUD",
	because it's taking forever to write.

	Originally I attempted to build on SocketMUD, and had written an ANSI color patch for it, but
	I found the codebase a little too limiting. For a time, this code was a part of SIL ("Simple
	Interpreted Language"), a project I started with Jeff Moyer, who was writing the interpreter.
	After that project fell to the wayside, I took it upon myself to learn POSIX threads, and added
	threading. Since then many additional features have been added, and most of them have been a learning
	experience for me.

	\section credit_sec Credit
	As mentioned before, SocketMUD gave me direction early on as I was trying to get the big picture
	into my mind. I'd like to thank Brian B. Graversen (http://www.socketmud.dk/) for writing and
	maintaining it. The only idea I glommed from SocketMUD was the separate threaded reverse-DNS lookup,
	because it was such a good idea!

	Many ideas were also born out of reading Designing Virtual Worlds by Richard Bartle.

	\section ded_sec Dedication
	I'd like to thank Richard Bartle and Roy Trubshaw for their foresight in pioneering virtual worlds.
	Dr. Bartle also deserves thanks for treating my wife and I to fish and chips and a personal tour of
	London. I hope to be able to return the favor some day.

*/
