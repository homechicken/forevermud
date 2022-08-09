#ifndef MUD_CONNECTION_H
#define MUD_CONNECTION_H

#include <yaml-cpp/yaml.h>
#include <boost/format.hpp>

#include "mudconfig.h"
#include "client_socket.h"

#include "connectionState.h"

void *reverseDNSLookup(void *arg);

/// a class to handle lower-level connection stuff
/** This class handles some of the connection functions for a Player, which
	should inherit form this class.
*/
class Connection {
public:
	typedef enum {
		ConnState_Closed = 0,
		ConnState_Login,
		ConnState_Password,
		ConnState_Create,
		ConnState_Play
	} ConnStateEnum;

	Connection();
	virtual ~Connection();

	/// sets the file descriptor for the player's connection
	void setFd(const int fd) { mSocket.set_fd(fd); }
	/// gets the file descriptor for the player's connection
	int getFd() const { return mSocket.get_fd(); }

	/// sets the player's IP address
	void setHostIP(const std::string &ip);
	/// gets the player's IP address
	std::string getHostIP() const { return mIp; }

	/// sets the player's DNS-resolved hostname
	void setHostname(const std::string &hostname) { mHostname = hostname; }
	/// gets the player's DNS-resolved hostname
	std::string getHostname() const { return mHostname; }

	/// true if the player is on a non-private IP address
	bool hasPublicIPAddress() const { return mUsingPublicIPAddress; }

	/// flush the outgoing stream down the player's socket
	bool Flush() { return mSocket.flush(); }

	/// sets the player's next command
	std::string getNextCommand();

	/// gets the player's input buffer
	void getInBuffer(std::string &in) { mSocket.get_in_buffer(in); }

	/// gets the time the player logged on
	time_t getLogonTime() const { return mLogonTime; }

	/// resets the time the player last sent a command
	void resetLastCommandTime() { mLastCommandTime = time(NULL); }
	/// gets the time the player last sent a command
	time_t getLastCommandTime() const { return mLastCommandTime; }

	/// adds text to the input buffer
	void appendToInBuffer(const std::string &buf) { mSocket.from_client(buf); }

	/// sets whether or not the player doesn't want to see ANSI color
	void setColorBlindness(bool isBlind) { mSocket.setColorblind(isBlind); }
	/// an overloaded version to set color blindness by string
	void setColorBlindness(const std::string &isBlind);
	/// gets whether or not the client is color blind
	bool getColorBlindness() const { return mSocket.is_colorblind(); }

	/// sets the player's screen resolution
	bool setResolution(const int x, const int y);
	/// gets the player's configured terminal width
	int getResolutionX() const { return mResolutionX; }
	/// gets the player's configured terminal height
	int getResolutionY() const { return mResolutionY; }

	bool Read();

	void Write(const std::string &txt);
	void Write(const boost::format &txt);
	void Write(const StringVector &list);

	/// passes ANSI color parsing on to ClientSocket::parseColor()
	std::string parseColor(const std::string &txt) { return mSocket.parseColor(txt); }

	void connectionSave(YAML::Emitter &out) const;
	bool connectionLoad(const YAML::Node &node);

	void setConnectionState(ConnStateEnum state);

	/// passes the command through to the dynamic handler
	void process(const std::string &command) { mConnectionState->process(command); }

	bool inPlayState() const { return mConnectionState->inPlayState(); }

private:
	std::string mIp;	///< Source of connected IP
	std::string mHostname;	///< Resolved name of connected IP, if public
	bool mUsingPublicIPAddress;	///< Whether or not the connection is from a public IP address

	ClientSocket mSocket;	///< Basic communication functionality

	boost::shared_ptr<ConnectionState> mConnectionState;	///< What state is the player in?

	StringVector mCommandQueue;	///< holds upcoming commands from the client

	time_t mLogonTime;	///< Used to determine how long a connection has been open
	time_t mLastCommandTime;	///< Used to generate idle time

	int mResolutionX;	///< The player's terminal x resolution
	int mResolutionY;	///< The player's terminal y resolution

	void parseBuffer();
};

#endif // MUD_CONNECTION_H
