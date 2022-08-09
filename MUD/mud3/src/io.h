#ifndef MUD_IO_H
#define MUD_IO_H

#include <string>

#include "mudconfig.h"
#include "mudsql.h"
#include "fileio.h"

/// an abstracted class to interact with a storage device
/** This class lets you interact with storage devices, either the filesystem or
	a database. If you write your own input/output layers for a different system,
	you only have to add them as private classes and write a getResource and a
	saveResource function.
*/
class IO {
public:
	/// defines the different APIs available for the I/O system
	typedef enum {
		File = 0
#if USE_MYSQL
		,MySQL
#endif
	} IOType;

	IO();
	~IO();

	std::string getResource(const IOResourceLocator &loc);

	bool saveResource(const IOResourceLocator &loc, const std::string &data);

	StringVector getZoneList() const;
	StringVector getRoomsForZone(const std::string &zoneName) const;

	StringVector getFilesIn(const std::string &path) const;
	StringVector getDirectoriesIn(const std::string &path) const;

private:
	IOType mType;	///< tells you which type of I/O system we're using
	FileIO mFileIO;	///< an instance of FileIO to get resources from the local filesystem
#if USE_MYSQL
	MudSQL mMysql_db;	///< an instance of MySQL_db to get resources from MySQL
#endif

};

#endif // MUD_IO_H
