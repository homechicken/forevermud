#include "container.h"
#include "utility.h"

#include "milestone.h"
#include "playerDatabase.h"
#include "player.h"
#include "room.h"
#include "collection.h"

#include "global.h"
extern Global glob;

/// Constructor
/** Default capacity is set to a relatively low number.
*/
Container::Container() {
	mCapacity = 5;
}

/// Destructor
/** Nothing to do here
*/
Container::~Container() {
}

/// this is how you put things in this container
/** Controls what can be put in this container. A search is made for an existing
	Collection to add to before the item is added to the container.
	@param item a copy of the item we want to place in the container
	\return true if the item was successfully placed inside
	\note If this function returns true, it is the caller's responsibility to remove \c item
		from the old location and to set the item's new ObjectLocation variable correctly.
*/
bool Container::containerAdd(Physical::PhysicalPointer item) {
	bool success = false;

	// check for an existing collection in the container to add on to
	Collection::CollectionPointer collection = boost::dynamic_pointer_cast<Collection>(item);

	if(collection) {
		std::vector<Physical::PhysicalPointer>::iterator it;

		for(it = mContents.begin(); it != mContents.end(); ++it) {
			// a case-insensitive comparison is not necessary here
			if((*it)->getName() == item->getName()) {
				Collection::CollectionPointer originalCollection = boost::dynamic_pointer_cast<Collection>(*it);

				if(originalCollection) {
					originalCollection->changeQuantity(collection->getQuantity());
					collection->setQuantity(0);
					success = true;
					break;
				}
			}
		}

		if(!success) {
			// no previous collection to add to
			mContents.push_back(item);
			success = true;
		}
	} else {
		if(mContents.size() < mCapacity) {
			mContents.push_back(item);
			success = true;
		}
	}

	return success;
}

/// this is how you remove things from this container
/** Controls the removal of objects from this container
	@param item a pointer to the item to be removed
	\return true if the item has been removed
*/
bool Container::containerRemove(Physical::PhysicalPointer item) {
	bool success = false;

	if(item) {
		glob.log.debug(boost::format("Container::remove: Container has %1% items in it") % mContents.size());

		mContents.erase(std::remove(mContents.begin(), mContents.end(), item), mContents.end());

		glob.log.debug(boost::format("Container::remove: Container now has %1% items in it") % mContents.size());

		success = true;
	}

	return success;
}

/// removes an object from the container by the object's name
/** This function removes an object from the container by the object's name.
	\note This is somewhat dangerous, as it will remove the first item that has the specified name,
	whether there are multiple matches or not.
*/
bool Container::containerRemove(const std::string &name) {
	bool success = false;

	glob.log.debug(boost::format("Container::containerRemove(): Calling me with %1%") % name);

	std::vector<Physical::PhysicalPointer>::iterator it;

	for(it = mContents.begin(); it != mContents.end(); ++it) {
		if(Utility::iCompare((*it)->getName(), name)) {
			success = containerRemove(*it);
			break;
		}
	}

	return success;
}

/// a function to change the capacity of this container
/** This function resets the container's capacity.
	@param capacity the new number of objects this container should be able to hold
	\return true if the container can be resized
	\note This function only returns false if the container has more objects in it
		than what you want to resize it to.
*/
bool Container::containerSetCapacity(const unsigned int capacity) {
	if(capacity < mContents.size()) {
		return false;
	}
	mCapacity = capacity;
	return true;
}

/// a function that looks for an object in this container
/** This function is pretty important, it is the only way you will be able to find
	something that may be stored in this container, or in a container this one has
	inside it.
	@param name the name of the object you're looking for
	@param deepFind true if you want to look recursively through all containers that may be here
	\return a vector of pointers to the objects that match the \b name parameter
	\note Objects are matched based on either their name \e or their alias, so multiple
		matches may be more common than you think!
*/
std::vector<Physical::PhysicalPointer> Container::containerFind(const std::string &name, bool deepFind) const {
	std::vector<Physical::PhysicalPointer> items;
	std::vector<Physical::PhysicalPointer>::const_iterator it;

	for(it = mContents.begin(); it != mContents.end(); ++it) {
		if(Utility::iCompare((*it)->getName(), name)) {
			items.push_back(*it);
		} else {
			// check for a nickname also
			if((*it)->areYouA(name)) {
				items.push_back(*it);
			}
		}
		// check for deep find and execute
		if(deepFind) {
			ContainerPointer c = boost::dynamic_pointer_cast<Container>(*it);
			if(c) {
				std::vector<Physical::PhysicalPointer> temp = c->containerFind(name, true);
				std::vector<Physical::PhysicalPointer>::const_iterator pos;
				for(pos = temp.begin(); pos != temp.end(); ++pos) {
					items.push_back(*pos);
				}
			}
		}
	}

	return items;
}

/// generates a list of all objects in this container
/** This function is extremely important, it will be called from many places for many
	different reasons. It generates a list of all items inside the container, which may
	be a box, a person, a room, etc.
	@param ignore The name of an object to ignore (such as a player) to avoid seeing it in the list
	\return a string listing the contents of the container. If any of the
		contents happen to be of class Living their description will include their state
		and posture.
*/
std::string Container::containerListContents(const std::string &ignore) const {
	if(mContents.size() == 0) {
		return "Nothing";
	}

	std::vector<Physical::PhysicalPointer>::const_iterator it;
	boost::shared_ptr<Living> living;

	StringVector inventory, extra;

	for(it = mContents.begin(); it != mContents.end(); ++it) {
		switch((*it)->getObjectType()) {
			case PlayerObject:
					extra.push_back((*it)->getBriefDescription());
				break;
			default:
				// attempt to get the basic name
				inventory.push_back((*it)->getBriefDescription());
		}
	}

	StringVector::const_iterator pos;
	unsigned int i = 0;

	std::stringstream str;

	for(pos = inventory.begin(); pos != inventory.end(); ++pos, ++i) {
		str << *pos;
		if(i < inventory.size() - 1) {
			str << ", ";
		}
	}

	str << END;

	for(pos = extra.begin(); pos != extra.end(); ++pos) {
		str << *pos << END;
	}

	return str.str();
}

/// simple function that resends messages to all objects inside the container
/** This function has one job: to send Message objects to all objects inside it.
*/
void Container::resendMessage(Message::MessagePointer message) {
	Player::PlayerPointer player;
	Room::RoomPointer room;
	ContainerPointer container;

	std::vector<Physical::PhysicalPointer>::iterator it;
	for(it = mContents.begin(); it != mContents.end(); ++it) {
		player = boost::dynamic_pointer_cast<Player>(*it);
		room = boost::dynamic_pointer_cast<Room>(*it);
		container = boost::dynamic_pointer_cast<Container>(*it);

		if(player) {
			player->processMessage(message);
			continue;
		}
		if(room) {
			room->processMessage(message);
			continue;
		}
		if(container) {
			container->resendMessage(message);
		}
	}
}

/// generates a string representation of the class
/** This function generates a string representing the object so it may be saved
	and reloaded later.
*/
void Container::containerSave(YAML::Emitter &out) const {
	out << YAML::Key << "Container" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "capacity" << YAML::Value << static_cast<int>(mCapacity);

	// this is where we save all contents as their own YAML nodes in sequence
	out << YAML::Key << "contents" << YAML::Value << YAML::BeginSeq;

	if(mContents.size() > 0) {
		glob.log.debug(boost::format("Container::containerSave(): There are %1% objects in this container") % mContents.size());

		int numSaved = 0;

		for(Contents::const_iterator it = mContents.begin(); it != mContents.end(); ++it) {
			if((*it)->getObjectType() != PlayerObject) {
				(*it)->Save(out);
				++numSaved;
				glob.log.debug(boost::format("Container::containerSave(): Saved %1%") % (*it)->getName());
			} else {
				glob.log.debug(boost::format("Container::containerSave(): Not saving player %1%") % (*it)->getName());
			}
		}

		if(numSaved == 0) {
			// the room only had players in it
			out << YAML::Null;
		}
	} else {
		out << YAML::Null;
	}

	out << YAML::EndSeq;

	out << YAML::EndMap;
}

/// changes the location of all contained objects
/** This function resets the location of all contained objects to the location
	provided. It's probably a good thing to call it after loading an object from
	storate.
	@param loc The new location for all contained objects
*/
void Container::reparentContents(const ObjectLocation &loc) {
	if(mContents.size() > 0) {
		for(Contents::iterator it = mContents.begin(); it != mContents.end(); ++it) {
			(*it)->setLocation(loc);
		}
	}
}

/// loads previously saved data
/** This function loads data that was previously saved into memory
	@param node a YAML::Node pointing to the beginning of the saved container data
	\return true if able to load
*/
bool Container::containerLoad(const YAML::Node &node) {
	bool success = false;
	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Container::containerLoad(): Received non-map YAML node!");
		return success;
	}

	try {
		node["capacity"] >> mCapacity;

		const YAML::Node &contentNode = node["contents"];

		for(YAML::Iterator it = contentNode.begin(); it != contentNode.end(); ++it) {
			if(YAML::IsNull(*it)) {
				// it took a while to track this issue down, make sure success is true if contents are null..
				success = true;
				break;
			}

			Physical::PhysicalPointer obj = glob.Factory.create(*it);

			if(obj) {
				if(containerAdd(obj)) {
					success = true;
				} else {
					glob.log.error("Container::containerLoad: Could not add object to container!");
				}
			} else {
				glob.log.error("Container::containerLoad: Factory returned a NULL object");
			}
		}
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Container::containerLoad(): YAML parser exception caught: %1%") % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Container::containerLoad(): YAML exception caught (no such map key!): %1%") % e.what());
	} catch(...) {
		glob.log.error("Container::containerLoad(): caught a fall-through exception");
	}

	return success;
}

/// tells all contents what temp to equalize to
/** This function adjusts the temperature of all contents of this room to match
	what the internal temp currently is.
*/
void Container::containerEqualizeTemp(const int temp) {
	for(Contents::iterator it = mContents.begin(); it != mContents.end(); ++it) {
		(*it)->equalizeTemperature(temp);

		if((*it)->getMatterState() == Physical::Destruct) {
			ObjectLocation loc = (*it)->getLocation();

			Message::MessagePointer message(new Message);

			message->setType(Message::Information);
			message->setFrom((*it)->getName());
			message->setBody("Destroyed by temperature outside operating parameters. *Poof*");

			if(loc.type == PlayerObject) {
				message->setRcpt(loc.location);
				glob.playerDatabase.deliverMessage(message);
			} else if(loc.type == RoomObject) {
				resendMessage(message);
			}

			containerRemove(*it);
		}
	}
}

/// does the container have anything in it?
/** This function lets the caller know whether or not the container is empty.
	It can be useful when looping through rooms or inventories and doing operations,
	as it will allow you to shortcut and skip empty containers.
	\return true if the container is empty
*/
bool Container::isEmpty() const {
	bool empty = true;

	if(mContents.size() > 0) {
		empty = false;
	}

	return empty;
}

