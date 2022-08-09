#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <ctime>
#include <cstring> // for memcpy

#include "mudconfig.h"

/// Basic socket object the server derives from
/** This class defines socket behavior that the SocketDriver needs
*/
class Socket {

public:
	Socket();
	virtual ~Socket();

	bool initialize();
	bool initialize(const unsigned short listen_port);

	/// gets the highest possible file descriptor from the operating system
	int get_fdmax() const { return mFdmax; }

	/// makes a copy of the current file descriptor set
	void copy_fdset(fd_set *set) { memcpy(set, &mFdset, sizeof(mFdset)); }

	/// gets the socket file descriptor
	int get_socket_fd() const { return mSocket_fd; }

	/// sets the port to bind to
	void set_port(const unsigned short port) { mPort = port; }
	/// sets the connection backlog queue size
	void set_backlog(const int bkLog) { mBacklog = bkLog; }

	int open_connection();
	int open_connection(struct sockaddr_in *sock);

	void close_connection(const int fd);

	std::string convert_time(time_t tSeconds) const;

protected:
	fd_set mFdset;	///< Current set of socket descriptors to poll

	int mSocket_fd;	///< This socket's descriptor

private:
	int mOptval;	///< Used to access option values for setsockopt()
	unsigned short mPort;	///< Defines the port the server listens on
	int mBacklog;	///< Socket connection backlog
	int mFdmax;	///< Stores results of getdtablesize()

	struct sockaddr_in mAddr;	///< a struct that holds socket address information
};

#endif // SOCKET_H
