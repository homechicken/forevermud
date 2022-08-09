#include "io.h"
#include "global.h"

extern Global glob;

/// Constructor
/** The constructor sets the default layer to use for loading and saving resources.
	It is defined in mudconfig.h
*/
IO::IO() {
	mType = IO_TYPE; // defined in conf/mudconfig.h
}

/// Destructor
/** Nothing done here. See a pattern yet?
*/
IO::~IO() {
}

/// a function to retrieve a resource
/** This function fetches the contents of a resource depending on what type of
	I/O we're configured to use.
	@param loc a struct IOResourceLocator defining the resource we're looking for
	\return the contents of the resource as retrieved from the relevant storage layer
*/
std::string IO::getResource(const IOResourceLocator &loc) {
	std::string data;

	switch(mType) {
	case File:
		data = mFileIO.getResource(loc);
		break;
#if USE_MYSQL
	case MySQL:
		data = mMysql_db.getResource(loc);
		break;
#endif
	default:
		glob.log.error("IO::getResource resource type not configured");
		data = IO_RESOURCE_NOT_FOUND;
	}

	return data;
}

/// a function to save data to a resource
/** This function saves data to a resource depending on what type of I/O we're
	configured to use.
	@param loc a struct IOResourceLocator defining the resource we'd like to save
	@param data the string you want to save
	\return the value of the saveResource() call from the relevant storage layer
*/
bool IO::saveResource(const IOResourceLocator &loc, const std::string &data) {
	bool status = false;

	switch(mType) {
	case File:
		status = mFileIO.saveResource(loc, data);
		break;
#if USE_MYSQL
	case MySQL:
		status = mMysql_db.saveResource(loc, data);
		break;
#endif
	default:
		glob.log.error("IO::saveResource resource type not configured");
	}

	return status;
}

/// a function to list the contents of the data/zones/ directory
/** This function returns a vector of strings listing the directory
	contents of the data/zones directory.
	\return a vector of strings listing the directory contents
*/
StringVector IO::getZoneList() const {
	StringVector zones;

	switch(mType) {
	case File:
		zones = mFileIO.getZoneList();
		break;
#if USE_MYSQL
	case MySQL:
		zones = mMysql_db.getZoneList();
		break;
#endif
	default:
		glob.log.error("IO::getZoneList() resource type not configured");
	}

	return zones;
}

StringVector IO::getRoomsForZone(const std::string &zoneName) const {
	StringVector rooms;

	switch(mType) {
		case File:
			rooms = mFileIO.getRoomsForZone(zoneName);
			break;
#if USE_MYSQL
		case MySQL:
			rooms = mMysql_db.getRoomsForZone(zoneName);
			break;
#endif
		default:
			glob.log.error("IO::getRoomsForZone() resource type not configured");
	}

	return rooms;
}

StringVector IO::getFilesIn(const std::string &path) const {
	StringVector files;

	switch(mType) {
	case File:
		files = mFileIO.getFilesIn(path);
		break;
#if USE_MYSQL
//	case MySQL:
//		files = mMysql_db.getFilesIn(path);
//		break;
#endif
	default:
		glob.log.error("IO::getFilesIn() resource type not available");
	}

	return files;
}

StringVector IO::getDirectoriesIn(const std::string &path) const {
	StringVector dirs;

	switch(mType) {
	case File:
		dirs = mFileIO.getDirectoriesIn(path);
		break;
#if USE_MYSQL
//	case MySQL:
//		dirs = mMysql_db.getDirectoriesIn(path);
//		break;
#endif
	default:
		glob.log.error("IO::getDirectoriesIn() resource type not available");
	}

	return dirs;
}

