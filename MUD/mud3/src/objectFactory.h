#ifndef OBJECTFACTORY
#define OBJECTFACTORY

#include <yaml-cpp/yaml.h>

#include "mudconfig.h"
#include "physical.h"

/// a class that creates objects from text streams saved to the storage system
/** This class receives loaded data from the storage layer and recreates the objects
	that were originally saved there. It always returns a Physical::PhysicalPointer,
	the most basic of all objects, and must be downcast to the expected type upon
	return.
*/
class ObjectFactory {
public:
	ObjectFactory();
	~ObjectFactory();

	Physical::PhysicalPointer create(const YAML::Node &node);
	Physical::PhysicalPointer create(ObjectType type);
private:

};

#endif // OBJECTFACTORY
