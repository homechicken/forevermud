#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sstream>

#include "socketDriver.h"
#include "thread_functions.h"

#include "global.h"

extern Global glob;

/// Constructor
/** Does nothing
*/
SocketDriver::SocketDriver() {
}

/// Destructor
/** The destructor closes all connections to the driver
*/
SocketDriver::~SocketDriver() {
	glob.log.debug("~SocketDriver is closing ALL connections!");
	glob.playerDatabase.closeAllConnections();
}

/// closes a connection by its file descriptor
/** This function shuts down a connection based on its file descriptor
	@param fd an int representation of the file descriptor
*/
void SocketDriver::shutdown_connection(const int fd) {
	glob.log.debug(boost::format("SocketDriver::shutdown_connection(): Closing connection on descriptor %1%") % fd);
	close_connection(fd);
}

/// Handles new, incoming player connections
/** This function accepts new connections and tells the PlayerDatabase to create
	a new Player. It checks for banned IPs and responds with the ban reason, and
	shutting down the connection if it is banned. It sets the connection to a
	non-blocking state.  It sets the Player's IP, and determines whether or not
	to perform a reverse-DNS lookup on the Player's hostname.
*/
void SocketDriver::new_connection() {
	int argp = 1;
	int new_fd = 0;

	ClientSocket cSocket;

	struct sockaddr_in sock;
	socklen_t socklen;

	// go back if our new socket fails to be created
	new_fd = open_connection(&sock);

	if(new_fd == -1) {
		glob.log.error("Bad socket descriptor in new_connection()");
		return;
	}

	Player::PlayerPointer player = Player::PlayerPointer(new Player());

	player->setFd(new_fd);
	player->setObjectType(PlayerObject);

	glob.playerDatabase.add(player);

	glob.log.debug(boost::format("SocketDriver::new_connection(): New incoming connection on socket descriptor %1%") % new_fd);

	// make the socket non-blocking
	ioctl(new_fd, FIONBIO, &argp);

	socklen = sizeof(sock);

	if(getpeername(new_fd, (struct sockaddr *)&sock, &socklen) < 0) {
		glob.log.error("SocketDriver::new_connection - getpeername lookup failed");
		player->setHostIP("0.0.0.0");
		player->setHostname("unknown");
	} else {
		// use IP as temporary hostname
		std::string ip = inet_ntoa(sock.sin_addr);
		player->setHostIP(ip);

		if(glob.banMap.isBanned(ip)) {
			glob.log.warn(boost::format("SocketDriver::new_connection(): Attempted login from %1% (banned)") % ip);

			player->Write(boost::format("Login not allowed: %1%") % glob.banMap.getBannedReason(ip));
			glob.playerDatabase.remove(player);

			return;
		}
	}

	// send greeting and allow login
	std::string greeting = glob.Config.getStringValue("Greeting");

	if(!greeting.empty()) {
		player->Write(greeting);
	}

	player->setConnectionState(Connection::ConnState_Login);

	std::string loginPrompt = glob.Config.getStringValue("LoginPrompt");

	if(loginPrompt.empty()) {
		loginPrompt = "Login: ";
	}

	player->Write(loginPrompt);
}
