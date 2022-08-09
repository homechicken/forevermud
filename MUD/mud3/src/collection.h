#ifndef MUD_COLLECTION_H
#define MUD_COLLECTION_H

#include "mudconfig.h"
#include <boost/shared_ptr.hpp>
#include <yaml-cpp/yaml.h>

/// a class that defines behavior of collections
/** This class defines the behavior of collections, groups of objects
	that can be split apart, such as \b coins.
*/

class Collection {
public:
	typedef boost::shared_ptr<Collection> CollectionPointer;

	Collection();
	virtual ~Collection();

	/// gets the number of items in this collection
	unsigned int getQuantity() const { return mQuantity; }

	/// sets the quantity to a specific value
	void setQuantity(const unsigned int quantity) { mQuantity = quantity; }

	bool changeQuantity(const int quantity);

	bool collectionLoad(const YAML::Node &node);

	void collectionSave(YAML::Emitter &out) const;

private:
	unsigned int mQuantity; ///< how many items are here

};

#endif // MUD_COLLECTION_H
