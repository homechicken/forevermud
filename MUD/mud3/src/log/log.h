#ifndef LOG_H
#define LOG_H

#include <string>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <boost/format.hpp>

#if USE_MYSQL_LOGGING
#include "mysql_db.h"
#endif

/// typedef to determine where the log output goes
typedef enum {
	None = 0,
	Stderr,
	File,
	MySQL
} LogType;

/// A logging class
/** This class lets you log at four different levels: debug, info, warn, and error.
	Each type of log can have a different destination, determined by LogType.
*/
class Log {
public:
	Log();
	~Log();
	
	/// sets the name of the logfile
	void setLogName(const std::string &filename) { mLogName = filename; }
	/// gets the name of the logfile
	std::string getLogName(){ return mLogName; }

	void close();

	void configureAll(LogType t, bool useTimestamps);

	// call these for logging information!
	void info(const std::string &);
	void warn(const std::string &);
	void error(const std::string &);
	void debug(const std::string &);

	void info(const boost::format &);
	void warn(const boost::format &);
	void error(const boost::format &);
	void debug(const boost::format &);
	

	// logging destinations
	void setInfoType(LogType t);
	void setWarnType(LogType t);
	void setErrorType(LogType t);
	void setDebugType(LogType t);
	
	// timestamp logs
	/// defines whether or not to timestamp 'Info' level log messages
	void setInfoStamp(bool b) { mInfoStamp = b; }
	/// defines whether or not to timestamp 'Warn' level log messages
	void setWarnStamp(bool b) { mWarnStamp = b; }
	/// defines whether or not to timestamp 'Error' level log messages
	void setErrorStamp(bool b) { mErrorStamp = b; }
	/// defines whether or not to timestamp 'Debug' level log messages
	void setDebugStamp(bool b) { mDebugStamp = b; }
	
private:
	LogType mInfoType, mWarnType, mErrorType, mDebugType; ///< definitions for each log type
	bool mInfoStamp, mWarnStamp, mErrorStamp, mDebugStamp; ///< whether each log type has a timestamp

	std::string mLogName;	///< the name of the log

	std::string formatTime();

	std::ofstream mLogStream;	///< a stream object for the log data

	pthread_mutex_t mBusy;	/// mutext to keep threads from colliding

	bool lock();
	bool unlock();

	std::string fixSQLQuote(const std::string &msg);
	
	// these are safe to call any time, they will check whether the file is
	// already open or if other logTypes are using the file before closing it
	void openLogFile();
	void closeLogFile();
};


#endif // LOG_H
