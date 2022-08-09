#include <sstream>
#include <errno.h>
#include <boost/cast.hpp>

#include "socket.h"

#include "global.h"

extern Global glob;

/// Constructor
/** The constructor sets a few default variables and clears out the file descriptor
	set for usage when the server starts.
	\todo Probably should remove deprecated fdmax variable and all references to it
*/
Socket::Socket() {
	mOptval = 1;
	mPort = MUDPORT;
	mBacklog = SOCKET_CONNECTION_BACKLOG;

	mFdmax = getdtablesize();

	if(mFdmax < 1) {
		glob.log.error("Socket: call to getdtablesize() failed");
		mFdmax = MAX_PROCESS_OPEN_DESCRIPTORS;
	}

	FD_ZERO(&mFdset);
}

/// Destructor
/** The destructor logs the fact that it's being shut down
*/
Socket::~Socket() {
	glob.log.debug(boost::format("Socket::Socket(): Closing down port %1%") % mPort);
	close(mSocket_fd);
}

/// an overloaded function that tells the server to listen on a port
/** This function is an overloaded version of initialize(), and takes an extra \c port
	argument that overrides the default port to listen to.
	@param listen_port the port number to listen to
	\return the value of initialize()
*/
bool Socket::initialize(const unsigned short listen_port) {
	mPort = listen_port;
	return initialize();
}

/// Creates and binds the socket, starts listening for connections
/** This function initializes the socket driver, logging \e any possible errors
	(show me someone else's MUD driver that gives you this much error detail, I dare you!)
	\return true if the server could bind to the port and is running
*/
bool Socket::initialize(void) {
	int error = 0;

	mSocket_fd = socket(SOCKET_DOMAIN, SOCK_STREAM, SOCKET_PROTOCOL);

	if(mSocket_fd == -1) {
		switch(errno) {
			case EPROTONOSUPPORT:
				glob.log.error("The protocol type of the specified protocol is not supported within this domain");
				break;
			case EAFNOSUPPORT:
				glob.log.error("The implementation does not support the specified address family.");
				break;
			case ENFILE:
				glob.log.error("Not enough kernel memory to allocate a new socket structure.");
				break;
			case EMFILE:
				glob.log.error("Process file table overflow.");
				break;
			case EACCES:
				glob.log.error("Permission to create a socket of the specified type and/or protocol is denied");
				break;
			case ENOBUFS:
			case ENOMEM:
				glob.log.error("Insufficient memory is available. The socket cannot be created until sufficient resources are free.");
				break;
			case EINVAL:
				glob.log.error("Unknown protocol, or protocol family not available.");
				break;
			default:
				glob.log.error("Unknown error value returned from socket() call.");
		}
		return false;
	}

	mAddr.sin_family = SOCKET_FAMILY;
	mAddr.sin_addr.s_addr = INADDR_ANY;
	mAddr.sin_port = boost::numeric_cast<unsigned short>(htons(mPort));

	error = setsockopt(mSocket_fd, SOL_SOCKET, SO_REUSEADDR, &mOptval, sizeof(mOptval));

	if(error == -1) {
		switch(errno) {
			case EBADF:
				glob.log.error("The argument socket_fd is not a valid descriptor.");
				break;
			case ENOTSOCK:
				glob.log.error("The argument socket_fd is a file, not a socket.");
				break;
			case ENOPROTOOPT:
				glob.log.error("The option is unknown at the level indicated.");
				break;
			case EFAULT:
				glob.log.error("The address pointed to by optval is not in a valid part of the process address space");
				break;
			case EINVAL:
				glob.log.error("optlen invalid in setsockopt.");
				break;
			default:
				glob.log.error("Unknown error value returned from setsockopt().");
		}

		return false;
	}

	error = bind(mSocket_fd, (struct sockaddr *)&mAddr, sizeof(mAddr));

	if(error == -1) {
		switch(errno) {
			case EBADF:
				glob.log.error("socket_fd is not a valid descriptor.");
				break;
			case EINVAL:
				glob.log.error("The socket is already bound to an address.");
				break;
			case EACCES:
				glob.log.error("The address is protected, and the user is not the super-user.");
				break;
			case ENOTSOCK:
				glob.log.error("Argument is a descriptor for a file, not a socket.");
				break;
			default:
				glob.log.error("Unknown error value returned from bind() call.");
		}

		return false;
	}

	error = listen(mSocket_fd, mBacklog);

	if(error == -1) {
		switch(errno) {
			case EADDRINUSE:
				glob.log.error("Another socket is already listening on the same port.");
				break;
			case EBADF:
				glob.log.error("The argument is not a valid descriptor.");
				break;
			case ENOTSOCK:
				glob.log.error("The argument is not a socket.");
				break;
			case EOPNOTSUPP:
				glob.log.error("The socket is not of a type that supports the listen operation.");
				break;
			default:
				glob.log.error("Unknown error value returned from listen() call.");
		}

		return false;
	}

	FD_SET(mSocket_fd, &mFdset);

	return true;
}

/// Accepts an incoming connection, returns the descriptor
/** This function is a no-argument version of the open_connection function that
	takes \c struct \c sockaddr_in pointer. It accepts an incoming connection and
	adds it to the file descriptor set
	\return an int representing the file descriptor just accepted
*/
int Socket::open_connection(void) {
	int temp_fd = 0;

	struct sockaddr_in sock;
	socklen_t socklen = sizeof(sock);

	temp_fd = accept(mSocket_fd, (struct sockaddr *)&sock, &socklen);

	if(temp_fd == -1) {
		glob.log.debug("Socket: call to accept() failed");
		return temp_fd;
	}

	FD_SET(temp_fd, &mFdset);

	return temp_fd;
}

/// Accepts an incoming connection, returns the descriptor
/** This function accepts an incoming connection based on the passed-in \c struct
	\c sockaddr_in pointer, and adds it to the file descriptor set
	@param sock a pointer to a \c sockaddr_in struct
	\return an int representing the file descriptor just added
*/
int Socket::open_connection(struct sockaddr_in *sock) {
	int temp_fd = 0;

	socklen_t socklen = sizeof(struct sockaddr_in);

	temp_fd = accept(mSocket_fd, (struct sockaddr *)sock, &socklen);

	if(temp_fd == -1) {
		glob.log.debug("Socket: call to accept() with sock failed");
		return temp_fd;
	}

	FD_SET(temp_fd, &mFdset);

	return temp_fd;
}

/// closes a connection based on its file descriptor
/** This function closes an open connection based on the file descriptor provided.
	It checks to make sure you're not trying to close the server down by mistake.
	@param fd the int representation of the file descriptor to close
*/
void Socket::close_connection(const int fd) {
	glob.log.debug(boost::format("Socket::close_connection(): Request to close descriptor %1%") % fd);

	// make sure we don't accidentally close the server
	if(fd != mSocket_fd && FD_ISSET(fd, &mFdset)) {
		FD_CLR(fd, &mFdset);
		close(fd);
	}
}
