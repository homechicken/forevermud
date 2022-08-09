#ifndef MUD_FILEIO_H
#define MUD_FILEIO_H

#include <string>
#include <sstream>
#include <fstream>
#include <pthread.h>

#include "mudconfig.h"

/// class for reading files from the disk
/** This class is used by the abstraction layer to read and write files from and to
	the local filesystem.
*/
class FileIO {
public:
	FileIO();
	~FileIO();

	std::string getResource(const IOResourceLocator &loc);
	bool saveResource(const IOResourceLocator &loc, const std::string &data);

	StringVector getZoneList() const;
	StringVector getRoomsForZone(const std::string &zoneName) const;

	StringVector getFilesIn(const std::string &path) const;
	StringVector getDirectoriesIn(const std::string &path) const;

private:
	pthread_mutex_t mReadLock;
	pthread_mutex_t mWriteLock;

	bool write(const std::string &file, const std::string &data);

	bool append(const std::string &file, const std::string &data);

	std::string read(const std::string &file);
};

#endif // MUD_FILEIO_H
