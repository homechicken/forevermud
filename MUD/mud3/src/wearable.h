#ifndef WEARABLE
#define WEARABLE

#include "mudconfig.h"

/// a class that defines locations an object may be worn at
/** This class implements behavior for an object that may be worn
*/
class Wearable {
public:
	Wearable();
	~Wearable();

	bool wearableAt(const std::string &location) const;

	bool wearableAdd(const std::string &location);
	bool wearableRemove(const std::string &location);

private:
	std::map<std::string, bool> mLocations;	///< the locations at which it may be worn
};


#endif // WEARABLE
