#include <sstream>
#include <vector>
#include <boost/format.hpp>

#include "banMap.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** The constructor defines our banmap data resource and loads it into the class. */
BanMap::BanMap() {
	mResloc.type = DataObject;
	mResloc.name = "banmap.yaml";
	load();
}

/// Destructor
/** The destructor just saves the current ban list. */
BanMap::~BanMap() {
	save();
}

/// Function for retrieving stored data to populate the ban map with
/** This function gets the banmap resource, discards comments, and adds
	valid ban records to the internal data structure. It got a lot simpler after adding
	the Utility namespace. Then it got manageable when I converted it to use YAML.
*/
void BanMap::load() {
	std::string banData = glob.ioDaemon.getResource(mResloc);
	if(banData == IO_RESOURCE_NOT_FOUND) {
		glob.log.warn("BanMap::load(): No banmap file found");
		return;
	}

	std::istringstream is(banData);

	try {
		YAML::Parser parser(is);
		YAML::Node doc;
		parser.GetNextDocument(doc);
		const YAML::Node &node = doc["BannedIPs"];

		std::string key, value;

		for(YAML::Iterator it = node.begin(); it != node.end(); ++it) {
			it.first() >> key;
			it.second() >> value;

			if(!mBannedList.insert(std::make_pair(key, value)).second) {
				glob.log.error(boost::format("BanMap::load: Failed to add ban for %1% on %2%") % key % value);
			} else {
				glob.log.debug(boost::format("BanMap::load: Loaded ban for %1% on %2%") % key % value);
			}
		}
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("YAML Parser exception: %1%") % e.what());
	} catch(YAML::BadDereference &e) {
		glob.log.error(boost::format("YAML Bad Dereference: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("YAML Map key not found: %1%") % e.what());
	} catch(YAML::RepresentationException &e) {
		glob.log.error(boost::format("YAML Representation exception: %1%") % e.what());
	} catch(YAML::Exception &e) {
		glob.log.error(boost::format("YAML exception: %1%") % e.what());
	} catch(...) {
		glob.log.error("Generic exception caught");
	}
}

/// Function for storing data currently in memory to permanent storage location
/** This function writes out the banned client information to preserve between reboots. */
void BanMap::save() const {
	YAML::Emitter out;
	out << YAML::BeginMap;

	out << YAML::Key << "Instructions" << YAML::Value;
	out << YAML::BeginSeq;

	out << "Any offline modifications you make to the banned list will be preserved,";
	out << "but any additional comments will be deleted for your convenience.";
	out << "Entries are IP address, ': ', and a reason. For example:";
	out << "10.0.0.1: Banned for being stupid";

	out << YAML::EndSeq;

	out << YAML::Key << "BannedIPs" << YAML::Value;
	out << YAML::BeginMap;

	if(mBannedList.size() > 0) {
		for(StringMap::const_iterator it = mBannedList.begin(); it != mBannedList.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
		}
	} else {
		out << YAML::Value << YAML::Null;
	}

	out << YAML::EndMap; // BannedIPs

	out << YAML::EndMap;

	if(!out.good()) {
		glob.log.error(boost::format("BanMap::save(): YAML Emitter is in a bad state: %1%") % out.GetLastError());
	} else {
		if(!glob.ioDaemon.saveResource(mResloc, out.c_str())) {
			glob.log.error("BanMap::save: failed to save the ban map");
		}
	}
}

/// Adds an IP and a reason to the banned IP list
/** This function first checks to see if a ban already exists on the IP,
	and if so sets the error message and returns false. If not, it adds an
	entry for the IP and attempts to write the changed data.
	@param ip The IP address to ban
	@param reason An optional reason the IP address is banned
	\return true if the IP address is banned
	\note If a connection comes from a banned IP, the user will be shown the reason
			if there is one, and the server will disconnect the socket.
*/
bool BanMap::ban(const std::string &ip, const std::string &reason) {
	bool status = false;

	StringMap::const_iterator pos = mBannedList.find(ip);
	if(pos != mBannedList.end()) {
		// this is a duplicate ban!
		setErrorMessage(boost::str(boost::format("Duplicate ban on %1% detected. Original reason is %2%") % ip % pos->second));
	} else {
		mBannedList[ip] = reason;
		save();
		status = true;
	}

	return status;
}

/// Removes a ban from an IP address
/** This function removes an IP address from the banned list and logs an error
	message if unable to do so (probably because the IP address isn't really
	in the list).

	@param ip The IP address that should be removed from the ban list
	\return true if the ban could be removed
	\note This function should only return false if the IP was not in the ban list.
*/
bool BanMap::removeBan(const std::string &ip) {
	bool status = false;
	StringMap::iterator pos = mBannedList.find(ip);
	if(pos != mBannedList.end()) {
		// found the match to remove
		mBannedList.erase(pos);
		save();
		status = true;
	} else {
		setErrorMessage(boost::str(boost::format("The specified IP address %1% is not on the banned clients list!") % ip));
	}

	return status;
}

/// Simple check to see if an IP address is banned
/** This function checks the list in memory for a matching IP, returns true
	if a ban is in place.

	@param ip an IP address to check against
	\return true if the IP is in the ban list and should be blocked
*/
bool BanMap::isBanned(const std::string &ip) const {
	StringMap::const_iterator pos = mBannedList.find(ip);
	if(pos != mBannedList.end()) {
		return true;
	} else {
		return false;
	}
}

/// Returns the reason why an IP address is banned
/** This function checks for an existing ban on an IP address. The returned
	std::string should be checked to see if its empty, but that shouldn't happen
	if you always call isBanned() first, and then call this if isBanned() returns
	true

	@param ip the IP address for which you want to get the reason
	\return the reason the address was banned (\e not required)
*/
std::string BanMap::getBannedReason(const std::string &ip) const {
	StringMap::const_iterator pos = mBannedList.find(ip);
	std::string reason;

	if(pos != mBannedList.end()) {
		reason = pos->second;
		if(reason.empty()) {
			reason = "Reason Not Specified";
		}
	}

	return reason;
}

/// Generates a list of all banned users and why
/** This function iterates through the banned list to show all banned IP addresses
	and the reason they've been banned.

	\return an END-delimited string of addresses and reasons
*/
std::string BanMap::listBannedUsers() const {
	std::stringstream s;
	for(StringMap::const_iterator pos = mBannedList.begin(); pos != mBannedList.end(); ++pos) {
		s << pos->first << " Reason: " << pos->second << END;
	}
	return s.str();
}
