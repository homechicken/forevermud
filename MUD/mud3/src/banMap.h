#ifndef MUD_BANMAP_H
#define MUD_BANMAP_H

#include <string>
#include <map>

#include "mudconfig.h"

/// A class for handling banned IP addresses
/** This class does all the work regarding banning IP addresses
	from connecting to the mud. It also loads and saves its state
	every time a change is made.
*/
class BanMap {
public:
	BanMap();
	~BanMap();

	/// Tells you what went wrong when another member returns false
	std::string getErrorMessage() { return mErrorMessage; }

	void load();
	void save() const;

	bool ban(const std::string &ip, const std::string &reason);
	bool removeBan(const std::string &ip);

	bool isBanned(const std::string &ip) const;
	std::string getBannedReason(const std::string &ip) const;

	std::string listBannedUsers() const;

private:
	IOResourceLocator mResloc;	///< A struct that holds the resource type and name for file I/O
	std::string mErrorMessage;	///< Stores a description of what went wrong

	/// A private function to store an error message locally
	void setErrorMessage(const std::string &msg) { mErrorMessage = msg; }

	StringMap mBannedList;	///< Mapping of Banned IPs to Reasons
};

#endif // MUD_BANMAP_H
