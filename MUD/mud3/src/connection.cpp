#include <pthread.h>	// for reverseDNSLookup()
#include <netdb.h>		// for gethostbyaddr_r
#include <arpa/inet.h>	// for inet_pton

#include "connection.h"
#include "utility.h"
#include "connStateClosed.h"
#include "connStateLogin.h"
#include "connStatePassword.h"
#include "connStateCreate.h"
#include "connStatePlaying.h"

#include "global.h"
extern Global glob;

class Player;

/// Constructor
/** The constructor sets up some default values that should be overwritten when
	the Player is loaded
*/
Connection::Connection() {
	mLogonTime = time(NULL);
	mLastCommandTime = time(NULL);
	mResolutionX = glob.Config.getIntValue("DefaultClientScreenX");
	mResolutionY = glob.Config.getIntValue("DefaultClientScreenY");
	mConnectionState = boost::shared_ptr<ConnectionState>(new ConnectionState_Closed());
}

/// Destructor
/** Does nothing
*/
Connection::~Connection() {
}

/// Set connection's source IP
/** This function records the IP address the connection is coming from, and
	determines whether it is from a public or private IP address, which decides
	whether a reverse-DNS lookup should be performed to get the real hostname.
	@param ip the IP address the connection is coming from
*/
void Connection::setHostIP(const std::string &ip) {
	mIp = ip;

	if(mIp.empty()) {
		// don't check on IP if we don't have one
		return;
	}

	glob.log.debug(boost::format("Player::setHostIP() received %1%") % ip);

	std::vector<std::string> vIp = Utility::stringToVector(ip, ".");

	int quad1, quad2, quad3;	// for /8, /16, and /24 network bit checking

	quad1 = Utility::toInt(vIp[0]);
	quad2 = Utility::toInt(vIp[1]);
	quad3 = Utility::toInt(vIp[2]);

	// check for private IP address
	switch(quad1) {
	case 0:		// 0/8 is reserved
	case 127:	// 127/8 is loopback block
	case 10:	// 10/8 is private - Class A
		mUsingPublicIPAddress = false;
		break;
	case 128:	// 128.0/16 is reserved
		if(quad2 == 0) {
			mUsingPublicIPAddress = false;
		}
		break;
	case 169:	// 169.254/16 is reserved - Class B
		if(quad2 == 254) {
			mUsingPublicIPAddress = false;
		}
		break;
	case 172:	// 172.16/12 is private - Class B
		if(quad2 > 15 && quad2 < 32) {
			mUsingPublicIPAddress = false;
		} else {
			mUsingPublicIPAddress = true;
		}
		break;
	case 191:	// 191.255/16 is reserved
		if(quad2 == 255) {
			mUsingPublicIPAddress = false;
		}
		break;
	case 192:
		switch(quad2) {
		case 0:	// 192.0.0/24 is reserved
			if(quad3 == 0) {
				mUsingPublicIPAddress = false;
			} else {
				mUsingPublicIPAddress = true;
			}
			break;
		case 168:	// 192.168/16 is private - Class C
			mUsingPublicIPAddress = false;
			break;
		default:
			mUsingPublicIPAddress = true;
		}
		break;
	case 223:	// 223.255.255/24 is reserved
		if(quad2 == 255 && quad3 == 255) {
			mUsingPublicIPAddress = false;
		} else {
			mUsingPublicIPAddress = true;
		}
		break;
	default:
		mUsingPublicIPAddress = true;
	}

	pthread_attr_t attr;
	pthread_t thread_lookup;

	// spawn a thread to handle the reverse DNS lookup
	if(mUsingPublicIPAddress) {
		glob.log.debug("Spawning a DNS lookup thread");
		// initialize and create lookup thread
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread_lookup, &attr, reverseDNSLookup, (void *)this);
	} else {
		glob.log.info(boost::format("Client descriptor %1% has a private IP address") % getFd());
		setHostname("Private");
	}
}

/// gets the next command
/** This function returns the next command the player sent to the driver.
	\return the text of the player's next command or a blank string if none
*/
std::string Connection::getNextCommand() {
	if(mCommandQueue.size() == 0) {
		return "";
	}

	StringVector::iterator it = mCommandQueue.begin();
	std::string cmd = *it;
	mCommandQueue.erase(it);

	return cmd;
}

/// sets whether the client wants to see ANSI color or not
/** This function allows a player to turn on or off ANSI color parsing for his/her
	connection.
	@param isBlind true if the client wants to see no color
*/
void Connection::setColorBlindness(const std::string &isBlind) {
	bool isColorBlind = false;
	if(Utility::iCompare(isBlind, "true")) {
		isColorBlind = true;
	}
	mSocket.setColorblind(isColorBlind);
}

/// sets the player's terminal X and Y resolution
/** This function sets the X and Y size of the screen the player has. It is mainly
	used in formatting lists (like in CommandHandler::getCommandList()) so they
	appear properly in the client terminal. It will not let you set them too low.
	@param x the width of the player's terminal in characters
	@param y the height of the player's terminal in characters
	\return true if the X and Y values are above values defined in mudconfig.h
*/
bool Connection::setResolution(const int x, const int y) {
	if(x < glob.Config.getIntValue("ClientScreenFloorX") || y < glob.Config.getIntValue("ClientScreenFloorY")) {
		return false;
	}
	
	mResolutionX = x;
	mResolutionY = y;
	
	return true;
}

/// read's from the player's socket
/** This function asks the ClientSocket object to read its input buffer, then
	pushes control over to parseBuffer() to process the results.
*/
bool Connection::Read() {
	bool result = mSocket.read_socket();
	parseBuffer();
	return result;
}

/// Sends text to the client and makes sure it gets written out
/** This is the only way to send players text. Use it wisely.
	@param txt the text you are sending to the client.
	\note The ClientSocket handles the actual ANSI color parsing
*/
void Connection::Write(const std::string &txt) {
	mSocket.to_client(txt, mResolutionX);
	mSocket.flush();
}

/// Overrides the normal Write to accept a boost::format instead
/** This function lets you send a string formatted on the fly with boost::format
	to the client.
	@param txt The format string to send to the client
*/
void Connection::Write(const boost::format &txt) {
	Write(boost::str(txt));
}

/// formats a StringVector into columns formatted for the client
/** This function takes a StringVector and formats it into columns according to the
	client's X resolution settings.
	@param list a StringVector to be formatted into columns
	\warning You don't want to send anything to this that may have color sequences
		in it because the columns won't be formatted properly.
*/
void Connection::Write(const StringVector &list) {
	mSocket.to_client(list, mResolutionX);
	mSocket.flush();
}

/// goes through the input buffer, extracts commands, adds them to the queue
/** This function takes the raw socket input, extracts multiple commands (if there
	are any) and adds them all to the command queue for later execution.
*/
void Connection::parseBuffer() {
	std::string buffer;
	mSocket.get_in_buffer(buffer);

	// remove any errant carriage-return characters (who hates Windows? Anyone?)
	buffer = Utility::stringReplace(buffer, "\r", "");

	StringVector clist = Utility::stringToVector(buffer, "\n");
	for(StringVector::iterator it = clist.begin(); it != clist.end(); ++it) {
		if((*it).length() > 0) {
			mCommandQueue.push_back(Utility::stringClean(*it));
		}
	}

	glob.log.debug(boost::format("CommandQueue has %1% commands queued up") % mCommandQueue.size());
}


/// private function for reverse DNS lookup functionality
/** This function runs as its own thread and looks up an IP address, attempting
	to resolve it to a friendly hostname. If successful, it populates a Player
	object with the proper hostname. Buffer sizes are defined in mudconfig.h.
*/
void *reverseDNSLookup(void *arg) {
	if(arg == NULL) {
		perror("NULL argument sent to lookup thread!");
		pthread_exit(0);
	}

	Connection *connection = (Connection *)arg;

	int error;
	char buffer[HOST_BUFFER_LENGTH] = {0}; // host database entries are stored here, 256 bytes is usually safe, 2k should be more than enough
	char ip[HOST_IP_LENGTH] = {0}; // should be 16 at most

	assert(connection->getHostIP().length() < HOST_IP_LENGTH);

	strcpy(ip, connection->getHostIP().c_str());

	struct hostent	fromHost,
					*from = &fromHost,
					ent;

	struct in_addr host;
	void *addr = &host;

	// initialize host structure
	memset(addr, 0, sizeof(struct in_addr));

	// convert dotted quad ip to network address structure in AF_INET (IPv4) family
	inet_pton(SOCKET_DOMAIN, ip, addr);

	// do the lookup and store the result at &from
	gethostbyaddr_r(addr, sizeof(struct in_addr), SOCKET_DOMAIN, &ent, buffer, sizeof(buffer), &from, &error);

	if(from != NULL && strlen(from->h_name) > 0) {
		connection->setHostname(from->h_name);
	}

	pthread_exit(0);
}

/// generates a string representation of the object
/** This function saves the text information for this object so
	it may be reloaded later.
	@param out A YAML::Emitter to send the data to
*/
void Connection::connectionSave(YAML::Emitter &out) const {
	out << YAML::Key << "Connection" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "x-resolution" << YAML::Value << mResolutionX;
	out << YAML::Key << "y-resolution" << YAML::Value << mResolutionY;
	out << YAML::Key << "colorblind" << YAML::Value << getColorBlindness();
	out << YAML::EndMap;
}

/// loads saved data
/** This function reloads previously saved data back into memory
	@param node A YAML::Node object that stores the connection information
	\return true if the data was loadable
*/
bool Connection::connectionLoad(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Connection::connectionLoad(): Node is not a map!");
		return success;
	}

	try {
		node["x-resolution"] >> mResolutionX;
		node["y-resolution"] >> mResolutionY;

		bool colorblind = false;
		node["colorblind"] >> colorblind;
		setColorBlindness(colorblind);

		success = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Player::playerLoad: YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Player::playerLoad: YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Player::playerLoad: caught a fall-through exception");
	}

	return success;
}

void Connection::setConnectionState(ConnStateEnum state) {
	bool success = false;

	switch(state) {
	case ConnState_Closed:
		mConnectionState = boost::shared_ptr<ConnectionState>(new ConnectionState_Closed());
		break;

	case ConnState_Login:
		mConnectionState = boost::shared_ptr<ConnectionState>(new ConnectionState_Login());
		success = true;
		break;

	case ConnState_Password:
		mConnectionState = boost::shared_ptr<ConnectionState>(new ConnectionState_Password());
		success = true;
		break;

	case ConnState_Create:
		mConnectionState = boost::shared_ptr<ConnectionState>(new ConnectionState_Create());
		success = true;
		break;

	case ConnState_Play:
		mConnectionState = boost::shared_ptr<ConnectionState>(new ConnectionState_Playing());
		success = true;
		break;

	default:
		glob.log.error(boost::format("Connection::setConnectionState(): Unrecognized connection state %1%") % state);
	}

	if(success) {
		mConnectionState->mPlayer = glob.playerDatabase.getPlayer(getFd());
	}
}
