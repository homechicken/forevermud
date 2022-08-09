#include "query.h"

/// constructor
/** This function just initializes some members to default values
*/
Query::Query() {
	mError = false;

}

/// destructor
/** does nothing
*/
Query::~Query() {
}

/// set the server
/** This function lets you set the SQL connection object. It will check to make sure
	the server is valid, and that it's connected. If not, an error flag will be raised
	and a message stored.
	@param sql pointer to a valid MySQL_Server object
	\return true if everything is working
*/
bool Query::setServer(boost::shared_ptr<MySQL_Server> sql) {
	if(sql) {
		mServer = sql;
	} else {
		mError = true;
		mErrorMessage = "Query was assigned a NULL server object";
	}

	if(!sql->isConnected()) {
		mError = true;
		mErrorMessage = "Query was assigned an unconnected server object";
	}

	return mError;
}

/// executes the query
/** This function actually executes a query and fetches the results, storing them
	locally.
	@param query the SQL query
	\return true if the query executed without error
*/
bool Query::execute(const std::string &query) {
	if(mServer->execute(query)) {
		switch(mServer->getQueryType()) {
			case MySQL_Server::Select:
				mServer->getResults(mResultArray);
				break;

			case MySQL_Server::Insert:
			case MySQL_Server::Update:
			case MySQL_Server::Delete:
			case MySQL_Server::Create:
			case MySQL_Server::Drop:
				mAffectedRows = mServer->getAffectedRows();
				break;

			default:
		}
	} else {
		mError = true;
		mErrorMessage = mServer->getErrorMessage();
	}
}

