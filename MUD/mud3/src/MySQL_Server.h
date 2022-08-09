#ifndef MYSQL_SERVER_H
#define MYSQL_SERVER_H

#include <mysql/mysql.h>
#include <string>
#include <boost/format.hpp>

#define USE_VERBOSE_LOGGING 1

#if USE_VERBOSE_LOGGING
#include <fstream>
#endif

class MySQL_Server {
public:
	MySQL_Server();
	MySQL_Server(const std::string &server, const std::string &user, const std::string &password, const std::string &database);

	~MySQL_Server();

	bool isConnected() const { return mIsConnected; }

	std::string getErrorMessage() const { return mErrorMessage; }

	MYSQL * getPointer() const { return mPSQL; }

	void setServerName(const std::string &server) { mServerName = server; }
	void setUserName(const std::string &user) { mUserName = user; }
	void setPassword(const std::string &password) { mPassword = password; }
	void setDatabase(const std::string &database) { mDatabase = database; }

	void connect();
	void disconnect();

	std::string escape(const std::string &text);

private:
	MYSQL mMySQL;
	MYSQL *mPSQL;

	std::string mServerName;
	std::string mUserName;
	std::string mPassword;
	std::string mDatabase;

	bool mIsConnected;

	std::string mErrorMessage;

	void initialize();

	void log(const boost::format &f);
	void log(const std::string &text);

#if USE_VERBOSE_LOGGING
	std::ofstream mLog;
#endif

};

#endif // MYSQL_SERVER_H
