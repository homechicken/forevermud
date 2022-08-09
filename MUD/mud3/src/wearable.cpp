#include "wearable.h"

/// Constructor
/** Does nothing
*/
Wearable::Wearable() {
}

/// Destructor
/** Does nothing
*/
Wearable::~Wearable() {
}

/// check if this item may be worn at a certain location
/** This function checks to see that the item may be worn at the specified
	location.
	@param location the name of the location to check for
	\return true if the item can be worn at that location
*/
bool Wearable::wearableAt(const std::string &location) const {
	std::map<std::string, bool>::const_iterator it = mLocations.find(location);

	if(it->second == true) {
		return true;
	}

	return false;
}

/// adds a location this item may be worn at
/** This function adds a new location at which this item may be worn.
	@param location the name of the location to add
	\return always returns true
*/
bool Wearable::wearableAdd(const std::string &location) {
	mLocations[location] = true;
	return true;
}

/// removes a location this item may be worn at
/** This function removes a previously-added location from the wearable list
	@param location the name of the location to remove
	\return always returns true
	\todo remove by iterator instead of setting the value to false?
*/
bool Wearable::wearableRemove(const std::string &location) {
	mLocations[location] = false;
	return true;
}
