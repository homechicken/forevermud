#ifndef MUDSQL_H
#define MUDSQL_H

#define MYSQL_LOG_TABLE			"log"
#define MYSQL_LOG_IDX			0
#define MYSQL_LOG_LEVEL			1
#define MYSQL_LOG_MESSAGE		2
#define MYSQL_LOG_DATE			3
#define MYSQL_LOG_IDX_TEXT		"idx"
#define MYSQL_LOG_LEVEL_TEXT	"level"
#define MYSQL_LOG_MESSAGE_TEXT	"message"
#define MYSQL_LOG_DATE_TEXT		"date"

#define MYSQL_PLAYER_TABLE			"player"
#define MYSQL_PLAYER_IDX			0
#define MYSQL_PLAYER_NAME			1
#define MYSQL_PLAYER_DATA			2
#define MYSQL_PLAYER_IDX_NAME_TEXT	"idx"
#define MYSQL_PLAYER_NAME_TEXT		"name"
#define MYSQL_PLAYER_DATA_TEXT		"data"

#define MYSQL_ZONE_TABLE		"zone"
#define MYSQL_ZONE_IDX			0
#define MYSQL_ZONE_NAME			1
#define MYSQL_ZONE_IDX_TEXT		"idx"
#define MYSQL_ZONE_NAME_TEXT	"name"

#define MYSQL_ROOM_TABLE		"room"
#define MYSQL_ROOM_IDX			0
#define MYSQL_ROOM_ZONE			1
#define MYSQL_ROOM_NAME			2
#define MYSQL_ROOM_DATA			3
#define MYSQL_ROOM_IDX_TEXT		"idx"
#define MYSQL_ROOM_ZONE_TEXT	"zone"
#define MYSQL_ROOM_NAME_TEXT	"text"
#define MYSQL_ROOM_DATA_TEXT	"data"

#define MYSQL_DATA_TABLE		"data"
#define MYSQL_DATA_NAME			0
#define MYSQL_DATA_DATA			1
#define MYSQL_DATA_NAME_TEXT	"name"
#define MYSQL_DATA_DATA_TEXT	"data"

#include <boost/shared_ptr.hpp>
#include "MySQL_Server.h"

/// A specific class to access MUD resources through MySQL
/** This class uses the base MySQL class to access MUD resources stored in a MySQL server
*/
class MudSQL {
public:
	MudSQL();
	~MudSQL();

	std::string getResource(const IOResourceLocator &loc) const;
	bool saveResource(const IOResourceLocator &loc, const std::string &data);

	StringVector getZoneList() const;
	StringVector getRoomsForZone(const std::string &zoneName) const;

	void getFilesIn(/*const std::string &t*/) const {}
	void getDirectoriesIn(/*const std::string &t*/) const { }

	int getZoneIndex(const std::string &zone_name) const;

	int getRoomIndex(const std::string &zone_name, const std::string &room_name) const;
	int getRoomIndex(const int zone_index, const std::string &room_name) const;

	int getPlayerIndex(const std::string &player_name) const;

private:
	boost::shared_ptr<MySQL_Server> mServer;
};

#endif // MUDSQL_H
