#ifndef MUD_CLIENT_SOCKET_H
#define MUD_CLIENT_SOCKET_H

#include <pthread.h>
#include <string>
#include <sstream>

#include "mudconfig.h"

/// Takes care of low-level connection needs for a player.
/** This class handles all input and output for a single, connected
	client. */
class ClientSocket {

public:
	ClientSocket();
	~ClientSocket();

	/// Sets the file descriptor
	void set_fd(const int fd) { mFd = fd; }

	/// Returns the file descriptor
	int get_fd() const { return mFd; }

	bool read_socket();
	bool flush();

	/// Fetches the socket's in buffer
	void get_in_buffer(std::string &buffer);

	void to_client(const std::string &text, int width);
	void to_client(const StringVector &list, int width);
	void from_client(const std::string &text);

	/// Whether we should parse ANSI color or ignore it
	bool is_colorblind() const { return mColorblind; }

	/// Set whether or not this connection can view ANSI color
	void setColorblind(bool isBlind) { mColorblind = isBlind; }

	bool lock();
	bool unlock();

	std::string parseColor(const std::string &txt);

private:
	int mFd;	///< This connection's socket descriptor

	std::string mIn_buffer;	///< Holds text received from this object
	std::string mOut_buffer;	///< Holds text waiting to be written out to this object

	pthread_mutex_t mBusy;	///< mutex to lock so threads don't fight over this resource

	bool mColorblind;	///< Whether the client can view ANSI color or not

	std::string formatWidth(const std::string &text, int width);

	std::string::size_type convertToken(const char *txt, std::stringstream &out);

	void processTelnetOptions(unsigned char *options);
};

#endif // MUD_CLIENT_SOCKET_H
