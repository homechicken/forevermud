#include <ctime>
#include <cstdlib>
#include <iostream>

#include "mudconfig.h"
#include "log.h"
#include "utility.h"

#if USE_MYSQL_LOGGING
/// \todo include mud_sql?
#endif

/// Constructor
/** The constructor initializes default values, setting all log output to std::err
*/
Log::Log() {
	// set default log destinations
	mInfoType = Stderr;
	mWarnType = Stderr;
	mErrorType = Stderr;
	mDebugType = Stderr;

	// set default log timestamp values
	mInfoStamp = true;
	mWarnStamp = true;
	mErrorStamp = true;
	mDebugStamp = true;

	// set default log file name
	mLogName = LOG_NAME;

	if(pthread_mutex_init(&mBusy, NULL) != 0) {
		perror("ClientScoket::ClientSocket(): mutex initialization error");
		exit(MUTEX_ERROR);
	}
}

/// Destructor
/** The destructor closes any open files or connections and exits
*/
Log::~Log() {
	if(mLogStream.is_open()) {
		mLogStream << formatTime();
		mLogStream << "ERROR: Forcing logStream closed from Log::~Log()!" << std::endl;
		mLogStream.close();
	}
}

/// close a file
/** This function closes an open file
*/
void Log::close() {
	if(mLogStream.is_open()) {
		mLogStream.close();
	}
}
/// locks the mutex
/** This function locks the mutext to prevent thread collisions
*/
bool Log::lock() {
	bool success = false;

	if(pthread_mutex_lock(&mBusy) != 0) {
		success = false;
	} else {
		success = true;
	}

	return success;
}

/// unlocks the mutex
/** This function unlocks the mutex so other threads can write to the log file
*/
bool Log::unlock() {
	bool success = false;

	if(pthread_mutex_unlock(&mBusy) != 0) {
		success = false;
	} else {
		success = true;
	}

	return success;
}

/// A quick way to configure all logs the same
/** This function configures all log output to use the same types, both output and
	timestamps
*/
void Log::configureAll(LogType t, bool useTimestamps) {
	setInfoType(t);
	setWarnType(t);
	setErrorType(t);
	setDebugType(t);
	mInfoStamp = useTimestamps;
	mWarnStamp = useTimestamps;
	mErrorStamp = useTimestamps;
	mDebugStamp = useTimestamps;
}

/// Sets information log's LogType
/** This function sets the information log's type
*/
void Log::setInfoType(LogType t) {
	if(mInfoType == File && t != File) {
		mInfoType = t;
		closeLogFile();
	}
	switch(t) {
		case None:
		case Stderr:
			mInfoType = t;
			break;
		case File:
			openLogFile();
			mInfoType = t;
			break;
#if USE_MYSQL_LOGGING
		case MySQL:
			mInfoType = t;
			break;
#endif
		default:
			std::cerr << "Log::setInfoType type not defined!" << std::endl;
			setInfoType(File); // default to File logging
	}
}

/// Sets warning log's LogType
/** This function sets the warning log's type
*/
void Log::setWarnType(LogType t) {
	if(mWarnType == File && t != File) {
		mWarnType = t;
		closeLogFile();
	}
	switch(t) {
		case None:
		case Stderr:
			mWarnType = t;
			break;
		case File:
			openLogFile();
			mWarnType = t;
			break;
#if USE_MYSQL_LOGGING
		case MySQL:
			mWarnType = t;
			break;
#endif
		default:
			std::cerr << "Log::setWarnType type not defined!" << std::endl;
			setWarnType(File); // default to File logging
	}
}

/// Sets error log's LogType
/** This function sets the error log's type
*/
void Log::setErrorType(LogType t) {
	if(mErrorType == File && t != File) {
		mErrorType = t;
		closeLogFile();
	}
	switch(t) {
		case None:
		case Stderr:
			mErrorType = t;
			break;
		case File:
			openLogFile();
			mErrorType = t;
			break;
#if USE_MYSQL_LOGGING
		case MySQL:
			mErrorType = t;
			break;
#endif
		default:
			std::cerr << "Log::setErrorType type not defined!" << std::endl;
			setErrorType(File); // default to File logging
	}
}

/// Sets debug log's LogType
/** This function sets the debug log's type
*/
void Log::setDebugType(LogType t) {
	if(mDebugType == File && t != File) {
		mDebugType = t;
		closeLogFile();
	}

	switch(t) {
		case None:
		case Stderr:
			mDebugType = t;
			break;
		case File:
			openLogFile();
			mDebugType = t;
			break;
#if USE_MYSQL_LOGGING
		case MySQL:
			mDebugType = t;
			break;
#endif
		default:
			std::cerr << "Log::setDebugType type not defined!" << std::endl;
			setDebugType(File); // default to File logging
	}
}

/// Logs information messages
/** This function writes a message out to the information log
*/
void Log::info(const std::string &t) {
	switch(mInfoType) {
		case None:
			break;
		case Stderr:
			if(lock()) {
				if(mInfoStamp) {
					std::cerr << formatTime();
				}
				std::cerr << "INFO: " << t << std::endl;
				unlock();
			}
			break;
		case File:
			if(lock()) {
				if(mInfoStamp) {
					mLogStream << formatTime();
				}
				mLogStream << "INFO: " << t << std::endl;
				unlock();
			}
			break;
#if USE_MYSQL_LOGGING
		case MySQL:
			std::stringstream s;
			s << "INSERT INTO " << MYSQL_LOG_TABLE << " VALUES (";
			s << "NULL, 'info', '" << fixSQLQuote(t) << "', NULL)";
			if(lock()) {
				mysql.query(s.str());
				unlock();
			}
			break;
#endif
		default:
			std::cerr << "Log::info type " << mInfoType << " not supported!" << std::endl;
			exit(LOGGING_ERROR);
	}
}

/// Logs warning messages
/** This function writes a message out to the warning log
*/
void Log::warn(const std::string &t) {
	switch(mWarnType) {
		case None:
			break;
		case Stderr:
			if(lock()) {
				if(mWarnStamp) {
					std::cerr << formatTime();
				}
				std::cerr << "WARN: " << t << std::endl;
				unlock();
			}
			break;
		case File:
			if(lock()) {
				if(mWarnStamp) {
					mLogStream << formatTime();
				}
				mLogStream << "WARN: " << t << std::endl;
				unlock();
			}
			break;
#if USE_MYSQL_LOGGING
		case MySQL:
			std::stringstream s;
			s << "INSERT INTO " << MYSQL_LOG_TABLE << "VALUES (";
			s << "NULL, 'warn', '" << fixSQLQuote(t) << "', NULL)";
			if(lock()) {
				mysql.query(s.str());
				unlock();
			}
			break;
#endif
		default:
			std::cerr << "Log::warn type " << mWarnType << " not supported!" << std::endl;
			exit(LOGGING_ERROR);
	}
}

/// Logs error messages
/** This function logs a message to the error log
*/
void Log::error(const std::string &t) {
	switch(mErrorType) {
		case None:
			break;
		case Stderr:
			if(lock()) {
				if(mErrorStamp) {
					std::cerr << formatTime();
				}
				std::cerr << "ERROR: " << t << std::endl;
				unlock();
			}
			break;
		case File:
			if(lock()) {
				if(mErrorStamp) {
					mLogStream << formatTime();
				}
				mLogStream << "ERROR: " << t << std::endl;
				unlock();
			}
			break;
#if USE_MYSQL_LOGGING
		case MySQL:
			std::stringstream s;
			s << "INSERT INTO " << MYSQL_LOG_TABLE << "VALUES (";
			s << "NULL, 'error', '" << fixSQLQuote(t) << "', NULL)";
			if(lock()) {
				mysql.query(s.str());
				unlock();
			}
			break;
#endif
		default:
			std::cerr << "Log::error type " << mErrorType << " not supported!" << std::endl;
			exit(LOGGING_ERROR);
	}
}

/// Logs debug messages
/** This function writes a message out to the debug log
*/
void Log::debug(const std::string &t) {
	switch(mDebugType) {
		case None:
			break;
		case Stderr:
			if(lock()) {
				if(mDebugStamp) {
					std::cerr << formatTime();
				}
				std::cerr << "DEBUG: " << t << std::endl;
				unlock();
			}
			break;
		case File:
			if(lock()) {
				if(mDebugStamp) {
					mLogStream << formatTime();
				}
				mLogStream << "DEBUG: " << t << std::endl;
				unlock();
			}
			break;
#if USE_MYSQL_LOGGING
		case MySQL:
			std::stringstream s;
			s << "INSERT INTO " << MYSQL_LOG_TABLE << "VALUES (";
			s << "NULL, 'debug', '" << fixSQLQuote(t) << "', NULL)";
			if(lock()) {
				mysql.query(s.str());
				unlock();
			}
			break;
#endif
		default:
			std::cerr << "Log::debug type " << mDebugType << " not supported!" << std::endl;
			exit(LOGGING_ERROR);
	}
}

/// Logs informational messages with boost::format
/** This function writes messages to the info log that are formatted with boost::format
*/
void Log::info(const boost::format &f) {
	info(boost::str(f));
}

/// Logs informational messages with boost::format
/** This function writes messages to the warning log that are formatted with boost::format
*/
void Log::warn(const boost::format &f) {
	warn(boost::str(f));
}

/// Logs informational messages with boost::format
/** This function writes messages to the error log that are formatted with boost::format
*/
void Log::error(const boost::format &f) {
	error(boost::str(f));
}

/// Logs informational messages with boost::format
/** This function writes messages to the debug log that are formatted with boost::format
*/
void Log::debug(const boost::format &f) {
	debug(boost::str(f));
}

/// generates a human-readable timestamp
/** This function generates a human-readable time stamp for use in logging
*/
std::string Log::formatTime() {
	std::stringstream s;

	time_t t = time(NULL);
	struct tm *now = localtime(&t);

	s << "[";

	// date
	s << now->tm_year + 1900 << ".";

	if(now->tm_mon < 9) {
		s << "0";
	}
	s << now->tm_mon + 1 << ".";

	if(now->tm_mday < 10) {
		s << "0";
	}
	s << now->tm_mday << " ";

	// time
	if(now->tm_hour < 10) {
		s << "0";
	}
	s << now->tm_hour << ":";

	if(now->tm_min < 10) {
		s << "0";
	}
	s << now->tm_min << ":";

	if(now->tm_sec < 10) {
		s << "0";
	}
	s << now->tm_sec;

	s << "] ";

	return s.str();
}

/// opens a log file
/** This function opens up a log file
*/
void Log::openLogFile() {
	if(!mLogStream.is_open()) { // don't open again if it's already open
		mLogStream.open(mLogName.c_str(), std::ios::app);
		if(!mLogStream.is_open()) {
			std::cerr << "Cannot open " << LOG_NAME << " for appending!" << std::endl;
			exit(LOGGING_ERROR);
		}
	}
}

/// closes a log file
/** This function closes a log file
*/
void Log::closeLogFile() {
	if(mLogStream.is_open()) {
		if(	mInfoType != File &&
			mWarnType != File &&
			mErrorType != File &&
			mDebugType != File
		) {
			mLogStream.close();
		}
	}
}

/// Makes unsafe text strings safe for SQL insert
/** This function replaces any single-quote marks with a second quote mark to make
	the string safe for SQL insertion
*/
std::string Log::fixSQLQuote(const std::string &msg) {
	std::string t = Utility::stringReplace(msg, "'", "''");
	return t;
}
