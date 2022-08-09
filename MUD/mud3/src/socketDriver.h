#ifndef MUD_SOCKET_DRIVER_H
#define MUD_SOCKET_DRIVER_H

#include <string>
#include <vector>

#include "socket.h"
#include "client_socket.h"
#include "player.h"

/// Defines added socket functionality for the MUD program
/** This class manages connections for the driver
*/
class SocketDriver : public Socket {
public:
	SocketDriver();
	virtual ~SocketDriver();

	void shutdown_connection(const int fd);
	void new_connection();

private:
};

#endif // MUD_SOCKET_DRIVER_H
