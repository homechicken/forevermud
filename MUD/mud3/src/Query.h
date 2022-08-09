#ifndef QUERY
#define QUERY

#include "MySQL_Server.h"

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

class Query {
public:
	~Query();

	static boost::shared_ptr<Query> execute(boost::shared_ptr<MySQL_Server> server, const std::string &query);

	std::string getQuery() const { return mQuery; }

	unsigned int getNumberOfResultRows() const { return mResults.size(); }
	unsigned int getNumberOfAffectedRows() const { return mAffectedRows; }

	unsigned int getLastInsertId();

	std::vector<std::string> getNextRow();
	std::vector<std::string> getColumnNames() { return mColumnNames; }

	void rewind() { mCurrentRow = 0; }
	void reset();

	bool hasError() const { return !mErrorMessage.empty(); }
	std::string getError() const { return mErrorMessage; }

private:
	Query();
	void initLog();

	void setServer(boost::shared_ptr<MySQL_Server> server) { mServer = server; }

	void setQuery(const std::string &query);

	void execute();

	boost::shared_ptr<MySQL_Server> mServer;

	std::string mQuery;

	std::string mErrorMessage;

	std::vector<std::string> mColumnNames;

	std::vector<std::vector<std::string> > mResults;

	unsigned int mAffectedRows;

	unsigned int mCurrentRow;

	unsigned int mLastInsertId;

	void log(const boost::format &f);
	void log(const std::string &text);

#if USE_VERBOSE_LOGGING
	std::ofstream mLog;
#endif
};

#endif // QUERY

