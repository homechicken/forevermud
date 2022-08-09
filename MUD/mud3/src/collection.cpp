#include <cmath>

#include "collection.h"
#include "utility.h"

#include "global.h"
extern Global glob;

Collection::Collection() {
	mQuantity = 0;
}

Collection::~Collection() {
}

/// change the quantity of items at this collection
/** This function changes the quantity of the items this collection
	stores. Sending a negative number will decrement the quantity.
	@param quantity how much you want to change, positive for adding, negative for removing
	\return true if the operation is successful
	\note The value will not be changed when false is returned.
	\note After calling this function, you should also call getQuantity() and if it returns 0,
		the object has been depleted and should be removed.
*/
bool Collection::changeQuantity(const int quantity) {
	if(quantity >= 0) {
		mQuantity += quantity;
		return true;
	}

	// if we were sent a negative number greater than our existing quantity we can't perform this op
	if(fabs(quantity) > mQuantity) {
		return false;
	} else {
		mQuantity += quantity;
		return true;
	}
}


void Collection::collectionSave(YAML::Emitter &out) const {
	out << YAML::Key << "Collection" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "quantity" << YAML::Value << static_cast<int>(mQuantity);
	out << YAML::EndMap;
}

bool Collection::collectionLoad(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Collection::collectionLoad(): Node is not a map!");
		return success;
	}

	try {
		int t;
		node["quantity"] >> t;
		mQuantity = static_cast<unsigned int>(t);

		success = true;
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Collection::collectionLoad(): YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Collection::collectionLoad(): YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Collection::collectionLoad(): caught a fall-through exception");
	}

	return success;
}
