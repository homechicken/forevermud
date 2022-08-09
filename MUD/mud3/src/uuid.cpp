#include "uuid.h"

#include <sstream>

UUID::~UUID() {
}

/// accessor that allows you to get the instance of this object
/** This function is the singleton accessor for the UUID object and ensures that only
	one copy of the object will ever live in memory.
*/
UUID* UUID::Instance() {
	static UUID instance;

	return &instance;
}

std::string UUID::toString(const boost::uuids::uuid &id) {
	std::stringstream s;

	s << id;

	return s.str();
}
