#ifndef MUD_CONTAINER_H
#define MUD_CONTAINER_H

#include <boost/shared_ptr.hpp>
#include <yaml-cpp/yaml.h>

#include "mudconfig.h"
#include "physical.h"
#include "message.h"

/// A class that defines behavior for objects that can have other objects in them
/** This class is extremely important, as many many objects in the server will inherit
	from it. Only functionality pertaining to containers should be implemented here
*/
class Container {
public:
	/// a typedef to make declaring shared pointers easy
	typedef boost::shared_ptr<Container> ContainerPointer;
	/// a typedef of 'contents' of this container
	typedef std::vector<Physical::PhysicalPointer> Contents;

	Container();
	virtual ~Container();

	bool containerAdd(Physical::PhysicalPointer item);
	bool containerRemove(Physical::PhysicalPointer item);
	bool containerRemove(const std::string &name);

	bool containerSetCapacity(const unsigned int capacity);

	/// tells you how many objects this container can hold
	int containerGetCapacity() const { return mCapacity; }

	/// tells you how many things are in this container
	int containerGetNumberOfContents() const { return mContents.size(); }

	std::vector<Physical::PhysicalPointer> containerFind(const std::string &name, bool deepFind = false) const;

	std::string containerListContents(const std::string &ignore = "") const;

	void resendMessage(Message::MessagePointer message);

	void containerSave(YAML::Emitter &out) const;

	bool containerLoad(const YAML::Node &node);

	void containerEqualizeTemp(const int temp);

	void reparentContents(const ObjectLocation &loc);

	bool isEmpty() const;

private:
	Contents mContents; ///< all the objects this one has inside it
	unsigned int mCapacity;	///< how many objects this container can hold
};

#endif // MUD_CONTAINER_H
