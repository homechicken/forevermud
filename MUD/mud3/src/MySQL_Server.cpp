#include "MySQL_Server.h"

MySQL_Server::MySQL_Server() : mPSQL(&mMySQL){
	initialize();
	mServerName = "localhost";
	mUserName = "root";
}

MySQL_Server::MySQL_Server(const std::string &server, const std::string &user, const std::string &password, const std::string &database) :
	mPSQL(&mMySQL),
	mServerName(server),
	mUserName(user),
	mPassword(password),
	mDatabase(database)
{
	initialize();
	connect();
}

void MySQL_Server::initialize() {
	mysql_init(mPSQL);
	mIsConnected = false;
}

MySQL_Server::~MySQL_Server() {
#if USE_VERBOSE_LOGGING
	if(mLog.is_open()) {
		mLog.close();
	}
#endif
}

void MySQL_Server::connect() {
#if USE_VERBOSE_LOGGING
	if(!mLog.is_open()) {
		mLog.open("server.log", std::ios::app);
	}
#endif

	if(!mIsConnected) {
		log(boost::format("Attempting to connect to %1%") % mServerName);
		if(!(mysql_real_connect(mPSQL, mServerName.c_str(), mUserName.c_str(), mPassword.c_str(), mDatabase.c_str(), 0, NULL, 0))) {
			log(boost::format("Connection failed (%1% %2% %3%): %4%") % mUserName % mPassword % mDatabase % mysql_error(mPSQL));
			mErrorMessage = mysql_error(mPSQL);
		} else {
			log("Connection established");
			mIsConnected = true;
		}
	}
}

void MySQL_Server::disconnect() {
	if(mIsConnected) {
		log("Closing MySQL connection");
		mysql_close(mPSQL);
	}
}

void MySQL_Server::log(const std::string &text) {
#if USE_VERBOSE_LOGGING
	if(mLog.is_open()) {
		mLog << text << '\n';
	}
#endif
}

void MySQL_Server::log(const boost::format &f) {
#if USE_VERBOSE_LOGGING
	log(boost::str(f));
#endif
}

std::string MySQL_Server::escape(const std::string &text) {
	if(text.empty()) {
		return "";
	}

	int safeSize = (text.length() * 2) + 1;

	char safe[safeSize];

	mysql_real_escape_string(mPSQL, safe, text.c_str(), text.length());

	return safe;
}
