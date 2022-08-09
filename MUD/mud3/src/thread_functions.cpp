#include <string>
#include <vector>
#include <sstream>

#include <assert.h>
#include <sys/time.h> // for gettimeofday
#include <unistd.h> // for usleep
#include <errno.h>

#include "utility.h"
#include "socketDriver.h"
#include "thread_functions.h"
#include "commandHandler.h"
#include "player.h"

#include "global.h"

extern Global glob;
#include <boost/uuid/uuid_io.hpp>
/** @file */

/// The SocketDriver's main process
/** This is the SocketDriver's main process. It loops every SOCKET_TIME_RESOLUTION
	microseconds using select(), and looks for incoming connections or sockets
	waiting to be read from.
	\note If this function returns, it's because we shut the driver down or crashed.
*/
void thread_driver_func() {
	int result = 0;

	struct timeval tv;

	fd_set fdset;

	// set a timeout?
	tv.tv_sec = 0;
	tv.tv_usec = SOCKET_TIME_RESOLUTION;

	while(glob.shutdownMUD == false) {
		glob.driver.copy_fdset(&fdset);

		result = select(glob.driver.get_fdmax(), &fdset, NULL, NULL, &tv);

		// some OS's (like linux) will modify tv to reflect leftover time, so we repair it
		tv.tv_sec = 0;
		tv.tv_usec = SOCKET_TIME_RESOLUTION;

		if(result == -1) {
			std::stringstream s;
			switch(errno) {
			case EBADF:
				s << "One or more of the file descriptor sets specified a file descriptor ";
				s << "that is not a valid open file descriptor.";
				glob.log.error(s.str());
				break;
			case EINTR:
				s << "The function was interrupted before any of the selected events occurred ";
				s << "and before the timeout interval expired.";
				glob.log.error(s.str());
				break;
			case EINVAL:
				s << "Invalid timeout interval, nfds argument out of range, or one of the file ";
				s << "descriptors refers to a stream or multiplexer that is linked downstream ";
				s << "from a multiplexer (this is bad).";
				glob.log.error(s.str());
			default:
				glob.log.error("Unknown error value returned from select().");
			}

			// close down the driver
			glob.shutdownMUD = true;
		}
		else if(result == 0) {
			// timeout occurred, just continue the loop
			continue;
		} else {
			// did we get a new connection?
			if(FD_ISSET(glob.driver.get_socket_fd(), &fdset)) {
				glob.log.debug("Driver Thread: New incoming connection");
				glob.driver.new_connection();
				continue;
			}

			// poll currently opened sockets
			for(int i = 0; i <= glob.playerDatabase.getHighestFd(); ++i) {
				Player::PlayerPointer player = glob.playerDatabase.getPlayer(i);

				if(!player) {
					// no such file descriptor logged in right now
					continue;
				}

				if(FD_ISSET(player->getFd(), &fdset)) {
					if(!player->Read()) {
						// can't read from the player, although we think the connection is open
						// this happens if you escape a telnet session and issue telnet a 'quit' command
						Message::MessagePointer message = Message::MessagePointer(new Message);
						message->setType(Message::Quit);
						message->setFrom(player->getName());
						message->setBody(boost::format("%1% has disconnected with extreme prejudice (connection lost)") % Utility::toProper(player->getName()));
						glob.playerDatabase.broadcast(message);

						glob.log.warn(boost::format("Driver Thread: Player %1% on descriptor %2% disconnected uncleanly") % player->getName() % player->getFd());
						glob.playerDatabase.remove(player);
						continue;
					}

					if(!player->Flush()) {
						glob.log.error("Player can't be flushed, removing object");
						glob.playerDatabase.remove(player);
					}
				} // if FD_ISSET
			} // for() polling loop
		} // else
	} // while not shutdown

	glob.log.info("Driver thread shutting down");
}

/// The command processor main thread
/** This function runs in its own thread, and processes all commands received from
	connected players. It sleeps most of the time, but is guaranteed to wake up and
	check for commands every TIME_RESOLUTION microseconds.
	@param arg ignored, but required because it's a thread
	\return a void pointer that is also ignored
*/
void *thread_process_func(void *arg) {
	struct timeval lastTime, currentTime, lastHeartbeat;
	gettimeofday(&lastTime, NULL);
	lastHeartbeat = lastTime;
	unsigned long shortestProcessingTime = 999999999; // an arbitrarily large magic number
	unsigned long longestProcessingTime = 0; // the not-arbitrary smallest unsigned long value

	while(glob.shutdownMUD == false) {
		gettimeofday(&currentTime, NULL);

		// if we haven't waited long enough, go to sleep
		unsigned long sleepTime = napTime(&currentTime, &lastTime);

		// log sleep time to the stats engine
		if(sleepTime > 0) {
			glob.statEngine.addSleepTime(sleepTime);
		}

		unsigned long procTime = TIME_RESOLUTION - sleepTime;

		if(procTime > 0 && procTime < shortestProcessingTime) {
			shortestProcessingTime = procTime;
			glob.log.info(boost::format("Process Thread: Driver set record shortest processing time: %1%") % procTime);
		}

		if(procTime < TIME_RESOLUTION && procTime > longestProcessingTime) {
			longestProcessingTime = procTime;
			glob.log.info(boost::format("Process Thread: Driver set record longest processing time: %1%") % procTime);
		}

		// reset sleep counter
		lastTime = currentTime;

		if(heartbeatCheck(&currentTime, &lastHeartbeat)) {
			heartbeat();
			lastHeartbeat = currentTime;
		}
		
		glob.playerDatabase.processCommands();
/*
		for(int i=0; i <= glob.playerDatabase.getHighestFd(); ++i) {
			Player::PlayerPointer player = glob.playerDatabase.getPlayer(i);
			if(!player) {
				// not a current valid client descriptor
				continue;
			}

			std::string command = player->getNextCommand();
			if(command.empty()) {
				// player hasn't sent a new command yet
				continue;
			}

			player->process(command);
		}
*/
	} // while not shutdown

	glob.log.info("Process Thread shutting down!");

	pthread_exit(0);
}

/// Decides how long processing thread should sleep
/** This function evaluates two timestamps and returns how long the thread should sleep
	before waking up and checking for work. The default values are defined in mudconfig.h.
	@param current the current \c timeval timestamp
	@param last the previous \c timeval timestamp
	\return the number of microseconds it slept
*/
unsigned long napTime(struct timeval *current, struct timeval *last) {
	unsigned long useconds = (((current->tv_sec - last->tv_sec) * 1000000) + current->tv_usec) - last->tv_usec;
	if(useconds < TIME_RESOLUTION) {
		// we need to sleep
		usleep(TIME_RESOLUTION - useconds);
		return TIME_RESOLUTION - useconds;
	}
	return 0;
}

/// Determines if it is time to run heartbeat()
/** This function evaluates two \c timeval timestamps and returns true if it's time
	to run heartbeat() again. The default values are defined in mudconfig.h.
	@param current a \c timeval stamp of the current time
	@param lastHeartbeat a \c timeval stamp of the last time heartbeat() was called
	\return true if it's time to fire off heartbeat() again
*/
bool heartbeatCheck(struct timeval *current, struct timeval *lastHeartbeat) {
	// returns true if HEARTBEAT_RESOLUTION time has passed
	unsigned long useconds = (((current->tv_sec - lastHeartbeat->tv_sec) * 1000000) + current->tv_usec) - lastHeartbeat->tv_usec;
	if(useconds > HEARTBEAT_RESOLUTION) {
		return true;
	}
	return false;
}

/// Called regularly to process game information
/** This function is the beating heart of the engine. Everything that is time-sensitive
	will be called from here
*/
void heartbeat() {
	glob.log.debug("Calling heartbeat");
	glob.eventDaemon.processEvents();
	glob.playerDatabase.callHeartbeats();
	glob.zoneDaemon.heartbeat();
}

/// this thread saves all the rooms
/** This function saves all rooms in all zones, if a flag is set. The function wakes
	up once per minute to check for the flag's presence. This flag should be set by the
	heartbeat() function and is cleared when the rooms are done saving.
	@param arg nothing
	\return nothing
*/
void *thread_saveRooms_func(void *arg) {
	while(!glob.shutdownMUD) {
		if(glob.saveRooms) {
			glob.log.info("Save thread is saving rooms...");
			glob.zoneDaemon.saveAllZones();
			glob.log.info("Save thread is done saving rooms");
			glob.saveRooms = false;
		}

		sleep(60); // sleep for 1 minute at a time
	}

	glob.log.debug("Save thread shutting down");

	pthread_exit(0);
}

