#include "Query.h"
#include <ctime>

Query::Query() {
	reset();
}

Query::~Query() {
#if USE_VERBOSE_LOGGING
	if(mLog.is_open()) {
		time_t now = time(NULL);
		mLog << "****************************\n";
		mLog << ctime(&now) << '\n';
		mLog << "****************************\n\n";
		mLog.close();
	}
#endif
}

void Query::initLog() {
#if USE_VERBOSE_LOGGING
	if(!mLog.is_open()) {
		mLog.open("query.log", std::ios::app);
	}
#endif
}

void Query::setQuery(const std::string &query) {
	mQuery = query;
	log(boost::format("Setting query to: %1%") % query);
}

void Query::reset() {
	log("Resetting all local variables");
	mQuery.clear();
	mErrorMessage.clear();
	mColumnNames.clear();
	mAffectedRows = 0;
	mResults.clear();
	mCurrentRow = 0;
	mLastInsertId = 0;
}

boost::shared_ptr<Query> Query::execute(boost::shared_ptr<MySQL_Server> server, const std::string &query) {

	boost::shared_ptr<Query> myQuery(new Query);

	myQuery->initLog();
	myQuery->setServer(server);
	myQuery->setQuery(query);

	myQuery->execute();

	return myQuery;
}

void Query::execute() {
	log("Beginning query execution");

	if(!mServer) {
		log("No server to connect to");
		mErrorMessage = "No server configured";
		return;
	}

	if(!mServer->isConnected()) {
		log("Server is not connected");
		mErrorMessage = mServer->getErrorMessage();
		return;
	}

	if(mQuery.empty()) {
		log("Query is empty");
		mErrorMessage = "No query to execute";
		return;
	}

	int qerror = mysql_real_query(mServer->getPointer(), mQuery.c_str(), mQuery.length());

	if(qerror) {
		log(boost::format("Query execution error: %1%") % mysql_error(mServer->getPointer()));
		mErrorMessage = mysql_error(mServer->getPointer());
		return;
	}

	log("Query executed");

	MYSQL_RES *res = mysql_store_result(mServer->getPointer());

	if(res) {
		log("Results fetched");

		// this next bit gets the names of all the columns
		MYSQL_FIELD *field = NULL;

		while((field = mysql_fetch_field(res))) {
			if(field->name) {
				mColumnNames.push_back(field->name);
			} else {
				mColumnNames.push_back("");
			}
		}

		unsigned int width = mysql_num_fields(res);

		log(boost::format("Results field is %1% wide") % width);

		std::vector<std::string> line;

		MYSQL_ROW row;

		while((row = mysql_fetch_row(res))) {
			for(unsigned int i=0; i<width; ++i) {
				// checking for NULL data is really important here
				if(row[i]) {
					log(boost::format("Adding cell data '%1%'") % row[i]);
					line.push_back(row[i]);
				} else {
					log("Got a blank cell");
					line.push_back("");
				}
			}

			log("Adding a result row");
			mResults.push_back(line);

			line.clear();
		}

		log("Freeing results");
		mysql_free_result(res);
	} else {
		log("Result pointer was NULL");

		// result pointer is NULL, check to see if it actually should be
		if(mysql_field_count(mServer->getPointer()) == 0) {
			// this was not a select query
			mAffectedRows = static_cast<unsigned int>(mysql_affected_rows(mServer->getPointer()));
			log(boost::format("%1% rows were affected by the query") % mAffectedRows);
		} else {
			mErrorMessage = mysql_error(mServer->getPointer());
			log(boost::format("ERROR: %1%") % mErrorMessage);
		}
	}
}

unsigned int Query::getLastInsertId() {
	unsigned int lastId = 0;

	if(mServer && mServer->isConnected()) {
		lastId = static_cast<unsigned int>(mysql_insert_id(mServer->getPointer()));
	}

	return lastId;
}


std::vector<std::string> Query::getNextRow() {
	std::vector<std::string> temp;

	if(mCurrentRow < mResults.size()) {
		temp = mResults[mCurrentRow];
		++mCurrentRow;
	}

	return temp;
}

void Query::log(const std::string &text) {
#if USE_VERBOSE_LOGGING
	if(mLog.is_open()) {
		mLog << text << '\n';
	}
#endif
}

void Query::log(const boost::format &f) {
#if USE_VERBOSE_LOGGING
	log(boost::str(f));
#endif
}

