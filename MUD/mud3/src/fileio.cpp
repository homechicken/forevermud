#include <sstream>
#include <boost/filesystem.hpp>
#include "fileio.h"

#include "global.h"
extern Global glob;

// this alias makes a lot of lines in this file shorter..
namespace bf = boost::filesystem;

/// Constructor
/** This constructor initializes the read and write mutexes (mutices?)
*/
FileIO::FileIO() {
	if(pthread_mutex_init(&mReadLock, NULL) != 0) {
		perror("FileIO: read lock mutex initialization error");
		exit(MUTEX_ERROR);
	}

	if(pthread_mutex_init(&mWriteLock, NULL) != 0) {
		perror("FileIO: write lock mutex initialization error");
		exit(MUTEX_ERROR);
	}
}

/// Destructor
/** Nothing happens here
*/
FileIO::~FileIO() {
}

/// writes data to the file
/** This function writes data to a file. A mutex is used to prevent multiple threads
	from calling this function at the same time.

	@param file file name to open for writing
	@param data string of data to write out
	\return Whether or not the operation was successful
*/
bool FileIO::write(const std::string &file, const std::string &data) {
	bool result = false;
	std::ofstream fout;

	if(pthread_mutex_lock(&mWriteLock) == 0) {
		fout.open(file.c_str(), std::ios::out | std::ios::trunc);
		if(fout.is_open()) {
			fout << data;
			fout.close();
			result = true;
		} else {
			glob.log.error(boost::format("FileIO::write(): Error opening file %1% for writing") % file);
		}
		pthread_mutex_unlock(&mWriteLock);
	} else {
		glob.log.error(boost::format("FileIO::write(): Error locking mutex for writing file %1%") % file);
	}
	return result;
}

/// appends data to the file
/** This function attempts to append data to the file specified
	@param file name of the file to append the data to
	@param data string of data to append to resource
	\return Whether or not the operation was successful
*/
bool FileIO::append(const std::string &file, const std::string &data) {
	bool result = false;
	std::ofstream fout;

	if(pthread_mutex_lock(&mWriteLock) == 0) {
		fout.open(file.c_str(), std::ios::out | std::ios::app);
		if(fout.is_open()) {
			fout << data;
			fout.close();
			result = true;
		} else {
			glob.log.error(boost::format("FileIO::append(): Error appending to file %1%") % file);
		}
		pthread_mutex_unlock(&mWriteLock);
	} else {
		glob.log.error(boost::format("FileIO::append(): Error locking mutex for appending file %1%") %  file);
	}
	return result;
}

/// reads the contents of a file
/** This function attempts to read the file identified by the \c file argument and
	returns its contents to the caller. On error, it returns an empty string.
	@param file the name of the file to read in
	\return The contents of the file or an error message
*/
std::string FileIO::read(const std::string &file) {
	std::stringstream s;

	if(pthread_mutex_lock(&mReadLock) == 0) {
		std::ifstream fin;
		fin.open(file.c_str(), std::ios::in);
		if(fin.is_open()) {
			s << fin.rdbuf();
			fin.close();
		} else {
			s << IO_RESOURCE_NOT_FOUND;
			glob.log.warn(boost::format("FileIO::read(): Error opening file %1% for reading") % file);
		}
		pthread_mutex_unlock(&mReadLock);
	} else {
		glob.log.error(boost::format("FileIO::read(): Could not lock mutex for reading file %1%") % file);
	}
	return s.str();
}

/// easier function to retrieve a resource from the filesystem
/** This function determines the storage directory for the different types of objects
	to load and retrieves it using the private functions.
	@param loc a struct IOResourceLocator that describes the resource you need
	\return the contents of the resource, or an empty string on error
*/
std::string FileIO::getResource(const IOResourceLocator &loc) {
	std::string data = IO_RESOURCE_NOT_FOUND;

	switch(loc.type) {
	case PlayerObject:
		data = read("../data/players/" + loc.name);
		break;
	case DataObject:
		data = read("../data/" + loc.name);
		break;
	case ZoneObject:
		if(!loc.meta.empty()) {
			std::string zoneFile = "../data/zones/" + loc.meta + "/" + loc.name;
			data = read(zoneFile);
		}
		break;
	case RoomObject:
		// in this case IOResourceLocator meta contains zone name
		if(!loc.meta.empty()) {
			data = read("../data/zones/" + loc.meta + "/rooms/" + loc.name);
		}
		break;
	default:
		glob.log.error(boost::format("FileIO::getResource(): Resource name: %1% (meta: %2%) not found") % loc.name % loc.meta);
	}

	return data;
}

/// easier function to write data out to the local filesystem
/** This function makes it easy to write data out to the local filesystem. The file
	will be overwritten, and not appended to.
	@param loc a struct IOResourceLocator that describes the resource you want to save
	@param data a copy of the data you want to store
	\return the value of write(data)
*/
bool FileIO::saveResource(const IOResourceLocator &loc, const std::string &data) {
	std::string filename;

	switch(loc.type) {
	case PlayerObject:
		filename = "../data/players/" + loc.name;
		break;
	case RoomObject:
		filename = "../data/zones/" + loc.meta + "/rooms/" + loc.name;
		break;
	case DataObject:
		filename = "../data/" + loc.name;
		break;
	default:
		glob.log.error("FileIO::saveResource(): Resource type not configured.");
		return false;
	}

	return write(filename, data);
}

/// fetch the names of all the zones
/**	This function gets a list of all the zones in the data/zones directory
	\note This function simply specifies the zone directory and passes the
	getDirectoriesIn() function results back to the caller.
	\return A StringVector of the available zones
*/
StringVector FileIO::getZoneList() const {
	return getDirectoriesIn("../data/zones");
}

/// fetch the names of all the rooms in the specified zone
/**	This function gets a list of all the rooms that are in the specified zone's room
	directory.
	@param zoneName The name of the zone to look in
	\return a StringVector containing all the names of the files in the rooms directory
*/
StringVector FileIO::getRoomsForZone(const std::string &zoneName) const {
	std::string roomPath = "../data/zones/" + zoneName + "/rooms";
	return getFilesIn(roomPath);
}

/// gets a list of all files in a directory
/** This fucnction gets a list of all files (excluding directories) in the
	specified directory.
	@param path A directory
	\return A StringVector of all files in the directory
*/
StringVector FileIO::getFilesIn(const std::string &path) const {
	StringVector files;
	bf::path p = path;

	if(!bf::exists(p)) {
		glob.log.error(boost::format("FileIO::getFilesIn(): No such path (%1%)") % path);
		return files;
	}

	if(bf::is_directory(p)) {
		bf::directory_iterator end_iter;
		for(bf::directory_iterator it(p); it != end_iter; ++it) {
			try {
				if(!bf::is_directory(it->status())) {
					files.push_back(it->path().filename());
				}
			} catch(const std::exception &ex) {
				glob.log.error(boost::format("Boost file exception caught: %1%") % ex.what());
			}
		}
	} else {
		glob.log.error("FileIO::getFilesIn(): Path is not a directory");
	}

	return files;
}

/// gets a list of all directories in a directory
/** This function gets a list of all directories (excludes files) in the specified
	directory.
	@param path A directory
	\return A StringVector listing all directories present
*/
StringVector FileIO::getDirectoriesIn(const std::string &path) const {
	StringVector dirs;
	bf::path p = path;

	if(!bf::exists(p)) {
		glob.log.error(boost::format("FileIO::getDirectoriesIn(): No such path: %1%") % path);
		return dirs;
	}

	if(bf::is_directory(p)) {
		bf::directory_iterator end_iter;
		for(bf::directory_iterator it(p); it != end_iter; ++it) {
			try {
				if(bf::is_directory(it->status())) {
					dirs.push_back(it->path().filename());
				}
			} catch(const std::exception &ex) {
				glob.log.error(boost::format("FileIO::getDirectoriesIn(): Boost file exception caught: %1%") % ex.what());
			}
		}
	} else {
		glob.log.error("getDirectoriesIn(): Path is not a directory");
	}

	return dirs;
}
