#include "objectFactory.h"
#include "utility.h"

#include "global.h"
extern Global glob;

#include "milestone.h"
#include "book.h"
#include "coin.h"

/// Constructor
/** Does nothing
*/
ObjectFactory::ObjectFactory() {
}

/// Destructor
/** Does nothing
*/
ObjectFactory::~ObjectFactory() {
}

/// turns a string of data into an object
/** This class takes saved text data and creates an object of the proper type,
	returning a base shared_ptr that will have to be downcasted to what you expect
	it to return.
	@param node a YAML::Node to load data from
	\return a shared_ptr of the object with data loaded, or NULL
	\note Make sure you assign the return value to an actual Physical::PhysicalPointer or else you may lose the pointer!
*/
Physical::PhysicalPointer ObjectFactory::create(const YAML::Node &node) {
	Physical::PhysicalPointer base;

	// return a NULL shared_ptr if we didn't get any data to load
	if(node.GetType() != YAML::CT_MAP) {
		return base;
	}

	std::string type;
	ObjectType objtype = UndefinedObject;

	try {
		node["Physical"]["object-type"] >> type;
		objtype = Utility::getObjectTypeFromString(type);
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("ObjectFactory::create(): YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("ObjectFactory::create(): YAML key not found: %1%") % e.what());
	} catch(YAML::Exception &e) {
		glob.log.error(boost::format("ObjectFactory::create(): YAML exception caught: %1%") % e.what());
	} catch(...) {
		glob.log.error("ObjectFactory::create(): caught a fall-through exception");
	}

	switch(objtype) {
		case MilestoneObject:
			base = boost::shared_ptr<Milestone>(new Milestone);
			base->Load(node);
			break;

		case BookObject:
			base = boost::shared_ptr<Book>(new Book);
			base->Load(node);
			break;

		case CoinObject:
			base = boost::shared_ptr<Coin>(new Coin);
			base->Load(node);
			break;

		default:
			glob.log.error(boost::format("ObjectFactory::create(): No such object type: %1%") % type);
	}

	return base;
}

Physical::PhysicalPointer ObjectFactory::create(ObjectType type) {
	Physical::PhysicalPointer base;

	switch(type) {
		case BookObject:
			base = boost::shared_ptr<Book>(new Book);
			break;
		case CoinObject:
			base = boost::shared_ptr<Coin>(new Coin);
			break;
		default:
			glob.log.error(boost::format("ObjectFactory::create(ObjectType): ObjectType %1% not configured") % type);
	}

	return base;
}
