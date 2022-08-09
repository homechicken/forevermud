#ifndef MUDCONFIG_H
#define MUDCONFIG_H

#include <string>
#include <sstream>
#include <vector>
#include <map>

#define MUDPORT	2600

#define END "\n"

#define MAX_LOGIN_ATTEMPTS	3

// Define the protocol to be used for communciation. Valid values are:
// AF_UNSPEC, AF_UNIX, AF_LOCAL, AF_INET, AF_AX25, AF_IPX, AF_APPLETALK,
// AF_NETROM, AF_BRIDGE, AF_ATMPVC, AF_X25, AF_INET6, AF_ROSE, AF_DECnet,
// AF_NETBEUI, AF_SECURITY, AF_KEY, AF_NETLINK, AF_ROUTE, AF_PACKET,
// AF_ASH, AF_ECONET, AFATMSVC, AF_SNA, AF_IRDA, AF_PPPOX, AF_WANPIPE,
// AF_BLUETOOTH, in case you were wondering...
#define SOCKET_DOMAIN	AF_INET
#define SOCKET_FAMILY	SOCKET_DOMAIN

// Protocols are defined in /etc/protocols,
// or http://www.iana.org/assignments/protocol-numbers
#define SOCKET_PROTOCOL	0

// How many connections do we let queue up before turning them away?
// If the socket domain is AF_INET and the backlog argument is greater
// than the constant SOMAXCONN (128 in Linux kernel 2.[02]), it is silently
// (and non-portably) truncated to SOMAXCONN.
#define SOCKET_CONNECTION_BACKLOG		5

#define MAX_PROCESS_OPEN_DESCRIPTORS	10

// These two defines are used in class ClientSocket
#define kMaxSocketBufferWriteSize		4096
#define kMaxSocketInputBufferLength		1024

//
// Exit codes
//
#define MUTEX_ERROR			1
#define SEMAPHORE_ERROR		2
#define MYSQL_ERROR			3
#define LOGGING_ERROR		4

// number of microseconds before a socket timeout. Should probably be less than TIME_RESOLUTION
// default value is 100000, or 0.1 seconds
#define SOCKET_TIME_RESOLUTION 100000

// number of microseconds that should be between processing loops (thread_functions.cpp)
// default is 200000, or 0.2 seconds
#define TIME_RESOLUTION 200000

// how often do we run the heartbeat (in microseconds)? This MUST BE GREATER than TIME_RESOLUTION
// the default value is 3000000, or 3.0 seconds
#define HEARTBEAT_RESOLUTION 3000000

// how big the host database entry should be in the lookup thread. 256 bytes is usually
// safe, so 2k ought to be plenty
#define HOST_BUFFER_LENGTH 2048

// how long of a buffer to use for IP addresses
#define HOST_IP_LENGTH 32

// separator to use between a listing of conditions or events. This character must not appear
// in any condition or event name, and should never be ':' either (it's used internally).
#define CONDITION_SEPARATOR "|"
#define EVENT_SEPARATOR "|"

// how many blank spaces should separate list items dynamically generated according
// to a players X resolution? (command list, channel list, etc)
#define FORMATTING_SPACE	2

//
// Logging Options
//

// set this to 'false' to disable debug logging
#define DEBUG	true

// where is the log directory (in relation to the /bin directory)
// make sure this ends with a trailing slash
#define LOG_PATH	"../log/"

// this is the log filename used if you switch to File logging in the log object
#define LOG_NAME	"mud.log"

// do we timestamp log entries?
#define LOG_STAMP	true

//
// IO System Settings
//

// define what IO API we want to use
// it is EXTREMELY important this matches a type in io.h's IOType enum
// if not, it probably won't compile and you'll be reading this to figure out why
// Default options are: File, MySQL
#define IO_TYPE File

// IO system resource not found text identifier
// Don't change this unless you know what you're doing!!
#define IO_RESOURCE_NOT_FOUND "Resource Not Found"

// MySQL Access
// create your database and tables with the following queries:
/*
CREATE DATABASE mud;

CREATE TABLE mud.log (
	idx INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
	level ENUM('info', 'warn', 'error', 'debug') NOT NULL DEFAULT 'info',
	message TEXT NOT NULL,
	date DATETIME
);

CREATE TABLE mud.player (
	idx INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
	name VARCHAR(255) NOT NULL,
	data TEXT NOT NULL
);

CREATE TABLE mud.zone (
	idx INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
	name VARCHAR(255) NOT NULL
);

CREATE TABLE mud.room (
 	idx INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
 	zone INT UNSIGNED NOT NULL,
 	name VARCHAR(255) NOT NULL,
	data TEXT NOT NULL
);

CREATE TABLE mud.data (
	name VARCHAR(255) NOT NULL,
	data TEXT NOT NULL
);

GRANT ALL PRIVILEGES ON mud.* TO 'mud'@'localhost' IDENTIFIED BY 'password';
*/

#define USE_MYSQL			false
#define USE_MYSQL_LOGGING	false
#define MYSQL_HOST			"localhost"
#define MYSQL_USER			"mud"
#define MYSQL_PASSWORD		"password"
#define MYSQL_DATABASE		"mud"

//
// some simple data structures we need just about everywhere
//

/// defines the different types of objects to be loaded and saved
// do not change the order of these, and only insert on to the end of the enum!
typedef enum {
	PlayerObject = 0,
	RoomObject,
	DataObject, // these reside in the top of the data directory, eg 'socials', 'banmap'
	MilestoneObject,
	ZoneObject,
	BookObject,
	CoinObject,

	UndefinedObject
} ObjectType;

/// a simple struct to help locate an object
typedef struct {
	ObjectType type;	///< type of file we're to load/save
	std::string name;	///< file/table name
	std::string meta;	///< meta information (eg. zone name for a room)
} IOResourceLocator;

/// this struct stores the virtual location of a virtual object (different from IOResourceLocator!)
typedef struct {
	ObjectType type;		///< the type of object we're located in
	std::string zone;		///< the zone where this item is, if needed
	std::string location;	///< the name of the object we're located in
} ObjectLocation;

/// enumerates directions and provides angles at the same time
typedef enum {
	North = 0,
	Northnortheast = 22,
	Northeast = 45,
	Eastnortheast = 67,
	East = 90,
	Eastsoutheast = 112,
	Southeast = 135,
	Southsoutheast = 157,
	South = 180,
	Southsouthwest = 202,
	Southwest = 225,
	Westsouthwest = 247,
	West = 270,
	Westnorthwest = 292,
	Northwest = 315,
	Northnorthwest = 337
} Direction;


// typedefs to save typing later
typedef std::vector<std::string> StringVector;
typedef std::map<std::string, std::string> StringMap;

#endif // MUDCONFIG_H
