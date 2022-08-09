#include "mudconfig.h"
#include "mudsql.h"
#include "utility.h"
#include "Query.h"

#include "global.h"
extern Global glob;

/// the constructor
/** The constructor sets up default connection parameters defined in mudconfig.h
*/
MudSQL::MudSQL() {
	mServer = boost::shared_ptr<MySQL_Server>(new MySQL_Server(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD, MYSQL_DATABASE));
}

/// the destructor
/** The destructor currently does nothing
*/
MudSQL::~MudSQL() {
}


/// gets a resource from the database
/** This function is called by the IO class to retrieve data from the MySQL server.
	It takes the IOResourceLocator, determines which SQL table the resource should be
	stored in, and gets the resulting text blob.
	@param loc a struct IOResourceLocator describing the type of resource to retrieve
	\return the resource string or IO_RESOURCE_NOT_FOUND (defined in mudconfig.h)
	\warning I've never actually used this class yet, so it needs to be tested and
	debugged if there are any problems.
*/
std::string MudSQL::getResource(const IOResourceLocator &loc) const {
	std::string data(IO_RESOURCE_NOT_FOUND);

	std::stringstream querystring;

	querystring << "SELECT * FROM ";

	switch(loc.type) {
		case PlayerObject:
			querystring << MYSQL_PLAYER_TABLE << " WHERE " << MYSQL_PLAYER_NAME_TEXT << " = '" << loc.name << "'";
			break;

		case RoomObject:
			// \todo have an option to use the index if it's available??
			querystring << MYSQL_ROOM_TABLE << " WHERE " << MYSQL_ROOM_ZONE_TEXT << " = '" << loc.meta << "' AND ";
			querystring << MYSQL_ROOM_NAME_TEXT << " = '" << loc.name << "'";
			break;

		case DataObject:
			querystring << MYSQL_DATA_TABLE << " WHERE " << MYSQL_DATA_NAME_TEXT << " = '" << loc.name << "'";
			break;

		default:
			return data;
	}

	boost::shared_ptr<Query> query = Query::execute(mServer, querystring.str());

	if(query->hasError()) {
		glob.log.error(boost::format("MudSQL::getResource(): query error: %1%") % query->getError());
		return data;
	}

	if(query->getNumberOfResultRows() == 1) {
		std::vector<std::string> row = query->getNextRow();

		switch(loc.type) {
			case PlayerObject:
				if(row.size() == 3) {
					data = row[MYSQL_PLAYER_DATA];
				} else {
					glob.log.error("MudSQL:getResource(): Player table row count mismatch");
				}
				break;

			case RoomObject:
				if(row.size() == 4) {
					data = row[MYSQL_ROOM_DATA];
				} else {
					glob.log.error("MudSQL::getResource(): Room table row count mismatch");
				}
				break;

			case DataObject:
				if(row.size() == 2) {
					data = row[MYSQL_DATA_DATA];
				} else {
					glob.log.error("MudSQL::getResource(): Data table row count mismatch");
				}
				break;

			default:
				glob.log.error(boost::format("MudSQL::getResource(): ResourceLocation type %1% not supported") % loc.type);
		}
	} else {
		glob.log.error(boost::format("MudSQL::getResource(): query (%1%) returned multiple matches when it shouldn't have") % query->getQuery());
	}

	return data;
}

/// the interface to the MySQL server that allows you to store data
/** This function is called by the IO class to store data on the MySQL server. It
	first checks to see if there is an old copy of the resource already on the server,
	if so it runs an \c UPDATE query, otherwise it runs an \c INSERT.
	@param loc the struct IOResourceLocator describing the type of resource to store
	@param data a string that defines that resource
	\return true if the query was able to run
*/
bool MudSQL::saveResource(const IOResourceLocator &loc, const std::string &data) {
	bool success = false;

	std::stringstream querystring;

	// we need to decide if this is an UPDATE or an INSERT query
	std::string currentResource	= getResource(loc);

	if(currentResource == IO_RESOURCE_NOT_FOUND) {
		querystring << "INSERT INTO ";

		switch(loc.type) {
			case PlayerObject:
				querystring << MYSQL_PLAYER_TABLE << "(" << MYSQL_PLAYER_NAME_TEXT << ", ";
				querystring << MYSQL_PLAYER_DATA_TEXT << ") VALUES (";
				querystring << "'" << mServer->escape(loc.name) << "', '" << mServer->escape(data) << "')";
				break;

			case RoomObject:
				querystring << MYSQL_ROOM_TABLE << "(" << MYSQL_ROOM_ZONE_TEXT << ", " << MYSQL_ROOM_NAME_TEXT;
				querystring << ", " << MYSQL_ROOM_DATA_TEXT << ") VALUES ('" << mServer->escape(loc.meta) << "', '" << mServer->escape(loc.name) << "', ";
				querystring << "'" << mServer->escape(data) << "')";
				break;

			case DataObject:
				querystring << MYSQL_DATA_TABLE << " VALUES ('" << mServer->escape(loc.name) << "', ";
				querystring << "'" << mServer->escape(data) << "')";
				break;

			default:
				glob.log.error("MudSQL::saveResource INSERT resource not configured");
				return success;
		}
	} else {
		querystring << "UPDATE ";

		switch(loc.type) {
			case PlayerObject:
				querystring << MYSQL_PLAYER_TABLE << " SET data = '" << mServer->escape(data) << "' ";
				querystring << "WHERE name = '" << mServer->escape(loc.name) << "' LIMIT 1";
				break;

			case RoomObject:
				querystring << MYSQL_ROOM_TABLE << " SET data = '" << mServer->escape(data) << "' ";
				querystring << "WHERE idx = '" << getRoomIndex(loc.meta, loc.name) << "' LIMIT 1";
				break;

			case DataObject:
				querystring << MYSQL_DATA_TABLE << " SET data = '" << mServer->escape(data) << "' ";
				querystring << "WHERE name = '" << mServer->escape(loc.name) << "' LIMIT 1";
				break;

			default:
				glob.log.error("MySQL_db::saveResource resource type not configured");
				return success;
			}
	}

	boost::shared_ptr<Query> query = Query::execute(mServer, querystring.str());

	if(query->hasError()) {
		glob.log.error(boost::format("MudSQL::saveResource(): Query (%1%) error: %2%") % query->getQuery() % query->getError());
		return success;
	}

	if(query->getNumberOfAffectedRows() != 1) {
		glob.log.error(boost::format("MudSQL::saveResource(): Affected rows error. Expected 1, got %1%") % query->getNumberOfAffectedRows());
	} else {
		success = true;
	}

	return success;
}

/// a function to get a list of all zones
/** This function generates a list of all zones available
	\return a vector of strings listing the contents of the SQL zones table
*/
StringVector MudSQL::getZoneList() const {
	std::stringstream querystring;
	StringVector zones;

	querystring << "SELECT * FROM " << MYSQL_ZONE_TABLE;

	boost::shared_ptr<Query> query = Query::execute(mServer, querystring.str());

	if(query->hasError()) {
		glob.log.error(boost::format("MudSQL::getZoneList(): Query (%1%) error: %2%") % query->getQuery() % query->getError());
		return zones;
	}

	std::vector<std::string> row;

	for(unsigned int i=0; i<query->getNumberOfResultRows(); ++i) {
		row = query->getNextRow();

		if(row.size() != 2) {
			glob.log.error("MudSQL::getZoneList(): Zone table size mismatch");
			return zones;
		}

		zones.push_back(row[MYSQL_ZONE_NAME]);
	}

	return zones;
}

StringVector MudSQL::getRoomsForZone(const std::string &zoneName) const {
	std::stringstream querystring;
	StringVector rooms;

	int zi = getZoneIndex(zoneName);

	if(zi == -1) {
		glob.log.error(boost::format("MudSQL::getRoomsForZone(): Could not find zone name %1%") % zoneName);
		return rooms;
	}

	querystring << "SELECT " << MYSQL_ROOM_NAME_TEXT << " FROM " << MYSQL_ROOM_TABLE;
	querystring << "WHERE " << MYSQL_ROOM_ZONE << " = " << zi;

	boost::shared_ptr<Query> query = Query::execute(mServer, querystring.str());

	if(query->hasError()) {
		glob.log.error(boost::format("MudSQL::getRoomsForZone(): Query (%1%) error: %2%") % query->getQuery() % query->getError());
		return rooms;
	}

	std::vector<std::string> row;

	for(unsigned int i=0; i<query->getNumberOfResultRows(); ++i) {
		row = query->getNextRow();

		if(row.size() != 1) {
			glob.log.error("MudSQL::getRoomsForZone(): Room query result row size mismatch");
			break;
		} else {
			rooms.push_back(row[0]);
		}
	}

	return rooms;
}

/// retrieve the table index for the specified zone name
/** This function looks for the index of a given zone name and returns it to the caller
	@param zone_name the name of the zone
	\return the associated SQL table index, or -1
*/
int MudSQL::getZoneIndex(const std::string &zone_name) const {
	int index = -1;

	std::stringstream querystring;

	querystring << "SELECT * FROM " << MYSQL_ZONE_TABLE << " WHERE ";
	querystring << MYSQL_ZONE_NAME_TEXT << " = '" << mServer->escape(zone_name) << "'";

	boost::shared_ptr<Query> query = Query::execute(mServer, querystring.str());

	if(query->hasError()) {
		glob.log.error(boost::format("MudSQL::getZoneIndex(): Query (%1%) error: %2%") % query->getQuery() % query->getError());
	} else {
		std::vector<std::string> row = query->getNextRow();

		if(row.size() > MYSQL_ZONE_IDX) {
			index = Utility::toInt(row[MYSQL_ZONE_IDX]);
		}
	}

	return index;
}

/// retrieve the table index for the specified room
/** This function gets the SQL table index for a specified room in a specified zone. It first
	converts the zone to an index then calls the overloaded version
	@param zone_name the name of the zone in which to look for the room
	@param room_name the name of the room
	\return the index of the requested room, or -1
*/
int MudSQL::getRoomIndex(const std::string &zone_name, const std::string &room_name) const {
	int index = -1;

	int zi = getZoneIndex(zone_name);

	if(zi != -1) {
		index = getRoomIndex(zi, room_name);
	}

	return index;
}

/// retrieve the table index for the specified room
/** This overloaded function gets the SQL table index for a specified room in a zone
	@param zone_index the index of the zone in which to look
	@param room_name the name of the room to look for
	\return the index of the request room, or -1
*/
int MudSQL::getRoomIndex(const int zone_index, const std::string &room_name) const {
	int index = -1;

	std::stringstream querystring;

	querystring << "SELECT * FROM " << MYSQL_ROOM_TABLE << " WHERE ";
	querystring << MYSQL_ROOM_NAME_TEXT << " = '" << mServer->escape(room_name) << "' AND ";
	querystring << MYSQL_ROOM_ZONE_TEXT << " = " << zone_index;

	boost::shared_ptr<Query> query = Query::execute(mServer, querystring.str());

	if(query->hasError()) {
		glob.log.error(boost::format("MudSQL::getRoomIndex(): Query (%1%) error: %2%") % query->getQuery() % query->getError());
		return index;
	}

	if(query->getNumberOfResultRows() == 1) {
		std::vector<std::string> row = query->getNextRow();

		if(row.size() > MYSQL_ROOM_IDX) {
			index = Utility::toInt(row[MYSQL_ROOM_IDX]);
		} else {
			glob.log.error("MudSQL::getRoomIndex(): Room table size mismatch");
		}
	} else {
		glob.log.error(boost::format("MudSQL::getRoomIndex(): Query %1% did not return expected data") % query->getQuery());
	}

	return index;
}

/// retrieve the specified player's table index
/** This function looks for a player name in the player table and returns its index
	@param	player_name the name of the player to look for
	\return the index of the player's record, or -1
*/
int MudSQL::getPlayerIndex(const std::string &player_name) const {
	int index = -1;

	std::stringstream querystring;
	querystring << "SELECT * FROM " << MYSQL_PLAYER_TABLE << " WHERE ";
	querystring << MYSQL_PLAYER_NAME_TEXT << " = '" << mServer->escape(player_name) << "'";

	boost::shared_ptr<Query> query = Query::execute(mServer, querystring.str());

	if(query->hasError()) {
		glob.log.error(boost::format("MudSQL::getPlayerIndex(): Query (%1%) error: %2%") % query->getQuery() % query->getError());
		return index;
	}

	if(query->getNumberOfResultRows() == 1) {
		std::vector<std::string> row = query->getNextRow();

		if(row.size() > MYSQL_PLAYER_IDX) {
			index = Utility::toInt(row[MYSQL_PLAYER_IDX]);
		} else {
			glob.log.error("MudSQL::getPlayerIndex(): Player table size mismatch");
		}
	} else {
		glob.log.error(boost::format("MudSQL::getPlayerIndex(): Query (%1%) did not return expected data") % query->getQuery());
	}

	return index;
}
