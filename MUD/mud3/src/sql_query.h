#ifndef QUERY_H
#define QUERY_H

#include <boost/shared_ptr.hpp>

#include "mysql_server.h"

/// handles a single SQL query
/** This class will execute a single query and store its results.
*/
class Query {
public:
	Query();
	~Query();

	bool setServer(boost::shared_ptr<MySQL_Server> sql);

	bool execute(const std::string &query);

	unsigned int getNumRows();
	unsigned int getAffectedRows();

private:
	std::string mQuery;	///< The SQL query text

	std::vector<std::vector<std::string> > mResultArray;	///< Storage for the query results

	bool mError;	///< whether or not an error has occurred
	std::string mErrorMessage;	///< if there's an error, this holds the text explanation

	boost::shared_ptr<MySQL_Server> mServer;	///< holds the server connection object
};

#endif // QUERY_H
