#include "room.h"
#include "utility.h"

#include "global.h"
extern Global glob;

/// Constructor
/** The constructor makes sure the object is initialized as a Room and sets
	a relatively high capacity for its container. It also created a mutex so multiple
	threads don't fight for protected resources.
	\todo should the capacity have a limit?
*/
Room::Room() {
	setObjectType(RoomObject);
	containerSetCapacity(50);
	mFileName = "Unset";
	mZoneName = "Unset";

	if(pthread_mutex_init(&mBusy, NULL) != 0) {
		perror("Room::Room(): mutex initialization error");
		exit(MUTEX_ERROR);
	}

	setLength(600);
	setWidth(600);
	setHeight(600);
	setTemperature(65);

	mChanged = false;
	mCurrentWeather = 0; // this is a char!

	mBriefDescription = "a room";
	mVerboseDescription = "This is a plain room";
}

/// Destructor
/*** Destroys the mutex created by the constructor
*/
Room::~Room() {
	pthread_mutex_destroy(&mBusy);
}

/// sets the internal name of this file/resource
/** This function sets the internal name of this file or resource (in the case of MySQL
	storage). It attempts to lock the mutex because it does make a change to the class data
	@param name The name of this resource
*/
void Room::setFileName(const std::string &name)	{
	if(lock()) {
		mFileName = name;
		unlock();
	}
}

/// sets the zone name for this room
/** This function sets the name of the zone to which this room belongs. It does so only after locking
	the mutex to prevent data from getting out of sync between threads.
	@param name the name of the zone to which this room belongs
*/
void Room::setZoneName(const std::string &name)	{
	if(lock()) {
		mZoneName = name;
		unlock();
	}
}

/// loads a room from a storage device
/** This function loads room data from a storage device
	\return true if able to load
*/
bool Room::Load() {
	bool success = false;

	if(mZoneName.empty() || mFileName.empty()) {
		glob.log.error("Room::Load(): Cannot load room due to a blank zone or room name");
		return success;
	}

	if(lock()) {
		IOResourceLocator res;
		res.type = RoomObject;
		res.meta = mZoneName;
		res.name = mFileName;

		std::string data = glob.ioDaemon.getResource(res);
		if(data == IO_RESOURCE_NOT_FOUND) {
			glob.log.error(boost::format("Room::Load(): Cannot load requested room %1% in zone %2%") % mFileName % mZoneName);
			unlock();
			return success;
		}

		std::istringstream is(data);

		try {
			YAML::Parser parser(is);
			YAML::Node doc;
			parser.GetNextDocument(doc);

			if(!physicalLoad(doc["Physical"])) {
				glob.log.error(boost::format("Room::Load(): cannot load physical node for %1%:%2%") % mZoneName % mFileName);
			} else if(!roomLoad(doc["Room"])) {
				glob.log.error(boost::format("Room::Load(): Cannot load room node for %1%:%2%") % mZoneName % mFileName);
			} else if(!containerLoad(doc["Container"])) {
				glob.log.error(boost::format("Room::Load(): Cannot load container node for %1%:%2%") % mZoneName % mFileName);
			} else {
				success = true;
			}
		} catch(YAML::ParserException &e) {
			glob.log.error(boost::format("Room::Load(%1%:%2%): Parser exception: %3%") % mZoneName % mFileName % e.what());
			success = false;
		} catch(YAML::BadDereference &e) {
			glob.log.error(boost::format("Room::Load(%1%:%2%): Bad Dereference: %3%") % mZoneName % mFileName % e.what());
			success = false;
		} catch(YAML::KeyNotFound &e) {
			glob.log.error(boost::format("Room::Load(%1%:%2%): Map Key not found: %3%") % mZoneName % mFileName % e.what());
			success = false;
		} catch(YAML::RepresentationException &e) {
			glob.log.error(boost::format("Room::Load(%1%:%2%): Representation exception: %3%") % mZoneName % mFileName % e.what());
			success = false;
		} catch(YAML::Exception &e) {
			glob.log.error(boost::format("Room::Load(%1%:%2%): YAML exception: %3%") % mZoneName % mFileName % e.what());
			success = false;
		} catch(...) {
			glob.log.error(boost::format("Room::Load(%1%:%2%): Generic exception caught") % mZoneName % mFileName);
			std::cout << "Generic exception caught";
			success = false;
		}

		unlock();
	}

	return success;
}

/// a fake load function
/** This function is only here to satisfy an ancestor's virtual function definition.
	@param node A YAML::Node that will be completely ignored
	\return false always
*/
bool Room::Load(const YAML::Node &node) {
	glob.log.error("Room::Load(YAML::Node) was called instead of Room::Load()");
	return false;
}

/// a fake save function
/** This function is here to satisfy an ancestor's virtual function definition.
	@param out A YAML::Emitter that will be ignored
	\return false always
*/
bool Room::Save(YAML::Emitter &out) const {
	glob.log.error("Room::Save(YAML::Emitter) was called instead of Room::Save()");
	return false;
}

/// loads the room-specific information from the retrieved save file
/** This function handles loading for the room-specific information stored in the save
	file. \see Room::Load() for the entire process
	@param node A YAML::Node pointing to the saved Room data
	\return Whether or not loading was successful
*/
bool Room::roomLoad(const YAML::Node &node) {
	bool success = false;

	if(node.GetType() != YAML::CT_MAP) {
		glob.log.error("Room::roomLoad(): Node is not a map!");
		return success;
	}

	try {
		node["x-coordinate"] >> mMapX;
		node["y-coordinate"] >> mMapY;

		if(const YAML::Node *pName = node.FindValue("brief-description")) {
			*pName >> mBriefDescription;
		}

		if(const YAML::Node *pName = node.FindValue("verbose-description")) {
			*pName >> mVerboseDescription;
		}

		if(!YAML::IsNull(node["items-of-interest"])) {
			const YAML::Node &itemsOfInterest = node["items-of-interest"];

			for(YAML::Iterator it = itemsOfInterest.begin(); it != itemsOfInterest.end(); ++it) {
				std::string key, value;
				it.first() >> key;
				it.second() >> value;
				addItemOfInterest(key, value);
			}
		}

		success = loadExits(node["exits"]);
	} catch(YAML::ParserException &e) {
		glob.log.error(boost::format("Room::roomLoad(%1%:%2%): YAML parser exception caught: %1%") % mZoneName % mFileName % e.what());
	} catch(YAML::KeyNotFound &e) {
		glob.log.error(boost::format("Room::roomLoad(%1%:%2%): Map key not found: %3%") % mZoneName % mFileName % e.what());
	} catch(...) {
		glob.log.error(boost::format("Room::roomLoad(%1%:%2%): caught a fall-through exception") % mZoneName % mFileName);
	}

	return success;
}

/// loads all exits for this room
/** This function takes care of loading all the exits for this room.
	@param node A YAML::Node where the exit information starts
	\return true if the exit loaded properly
*/
bool Room::loadExits(const YAML::Node &node) {
	if(YAML::IsNull(node)) {
		// there are no exits to load
		return true;
	}

	bool success = false;

	if(node.GetType() != YAML::CT_SEQUENCE) {
		glob.log.error("Room::loadExits(): Node is not a sequence!");
		return success;
	}

	for(unsigned int i=0; i<node.size(); ++i) {
		Exit::ExitPointer exit = Exit::ExitPointer(new Exit);

		success = exit->exitLoad(node[i]);

		if(!success) {
			glob.log.error("Room::loadExits(): Failure loading exits");
			break;
		}

		addExit(exit);
	}

	return success;
}

/// saves a room to a storage device
/** This function saves the room data out for loading later
	\return true if able to save
*/
bool Room::Save() {
	bool success = false;

	if(getFileName().empty() || getZoneName().empty()) {
		glob.log.error("Room::Save(): Empty file or zone name, not able to save room data!");
		return success;
	}

	IOResourceLocator resloc;
	resloc.type = RoomObject;
	resloc.name = mFileName;
	resloc.meta = mZoneName;

	YAML::Emitter out;

	if(lock()) {
		out << YAML::BeginMap;
		physicalSave(out);
		roomSave(out);
		containerSave(out);
		out << YAML::EndMap;

		if(!out.good()) {
			glob.log.error(boost::format("Room::Save(): YAML Emitter is no good: %1%") % out.GetLastError());
		}

		unlock();
		success = glob.ioDaemon.saveResource(resloc, out.c_str());
	} else {
		glob.log.error("Room::Save(): Could not lock object");
	}

	mChanged = false;

	return success;
}

/// generates save data for this specific room
/** This function saves the current room's data as text so that it may be
	restored later.
	@param out A YAML::Emitter to save the data to
*/
void Room::roomSave(YAML::Emitter &out) const {
	out << YAML::Key << "Room" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "x-coordinate" << YAML::Value << mMapX;
	out << YAML::Key << "y-coordinate" << YAML::Value << mMapY;

	out << YAML::Key << "brief-description" << YAML::Value << mBriefDescription;
	out << YAML::Key << "verbose-description" << YAML::Value << mVerboseDescription;

	out << YAML::Key << "items-of-interest" << YAML::Value;

	if(mItemsOfInterest.size() > 0) {
		out << YAML::BeginMap;

		for(StringMap::const_iterator it = mItemsOfInterest.begin(); it != mItemsOfInterest.end(); ++it) {
			out << YAML::Key << it->first << YAML::Value << it->second;
		}

		out << YAML::EndMap;
	} else {
		out << YAML::Null;
	}

	out << YAML::Key << "exits" << YAML::Value;

	if(mExits.size() > 0) {
		out << YAML::BeginSeq;

		for(std::vector<Exit::ExitPointer>::const_iterator it = mExits.begin(); it != mExits.end(); ++it) {
			(*it)->exitSave(out);
		}

		out << YAML::EndSeq;
	} else {
		out << YAML::Null;
	}

	out << YAML::EndMap;
}

/// gets a description of an \b Item \b of \b Interest
/** This function checks for the presence of a matching \b Item \b of \b Interest
	and if found returns its text description.
	@param item the name of the item to check for
	\return a string description of the item, or a blank string
*/
std::string Room::getItemOfInterest(const std::string &item) const {
	glob.log.debug(boost::format("Room::getItemOfInterest() is looking for a %1%") % item);

	std::string text = "";

	StringMap::const_iterator it = mItemsOfInterest.find(item);

	if(it != mItemsOfInterest.end()) {
		text = it->second;
	}

	glob.log.debug(boost::format("Room::getItemOfInterest() is returning '%1%'") % text);

	return text;
}

/// adds an \b Item \b of \b Interest to the Room
/** This function adds an \b Item \b of \b Interest to the Room
	@param item the name of the item
	@param text the description of the item
	\return true if able to insert the item
	\note If this function returns false, it's because there's already an item with
		the same name in the room!
*/
bool Room::addItemOfInterest(const std::string &item, const std::string &text) {
	if(mItemsOfInterest.insert(std::make_pair(item, text)).second) {
		return true;
	}

	return false;
}

/// generates a description of what's in the room
/** This function generates a description of what this room is by using the Physical
	objects's getVerboseDescription() and listing the names of the visible exits and
	all contents of the room
	@param ignore The name of an object (usually a player) to ignore in the list
	\return a string fully describing the room and its contents
*/
std::string Room::getFullDescription(const std::string &ignore) const {
	std::stringstream s;

	s << getVerboseDescription();

	// if there is more than 1 thing here (the player), show contents
	if(containerGetNumberOfContents() > 1) {
		s << END << "~b00Contents~res: " << containerListContents(ignore);
	}

	return s.str();
}

/// adds an Exit to this room
/** This function adds an exit to the room's Exit list.
	@param exit an Exit shared_ptr to add
	\return true if no other exit with the same name exists
*/
bool Room::addExit(Exit::ExitPointer exit) {
	bool result = false;

	Exit::ExitPointer e = getExit(exit->getName());

	if(e) {
		// can't have more than one exit with the same name!
		glob.log.error(boost::format("Room::addExit(): Room %1% tried to add another exit: %2%") % getName() % exit->getName());
	} else {
		mExits.push_back(exit);
		result = true;
	}

	return result;
}

/// gets a copy of an Exit for the specified direction
/** This function returns a copy of an Exit object for the direction specified, if
	one exists
	@param direction the direction of the Exit object
	\return a shared_ptr copy of the Exit or NULL
*/
Exit::ExitPointer Room::getExit(const std::string &direction) const {
	Exit::ExitPointer exit;

	std::vector<Exit::ExitPointer>::const_iterator it;

	for(it = mExits.begin(); it != mExits.end(); ++it) {
		if(Utility::iCompare((*it)->getName(), direction)) {
			exit = *it;
		}
	}

	return exit;
}

/// generates a string of all visible exits
/** This function generates a space-delimited list of all visible exit directions
	currently in the exit list.
	\return a string of all visible exit directions
*/
std::string Room::getExitString() const {
	std::vector<Exit::ExitPointer>::const_iterator it;
	std::stringstream s;

	// the following variables are for separating the exit names but not adding
	// a final space at the end
	unsigned int numExits = mExits.size();
	unsigned int i = 0;

	for(it = mExits.begin(); it != mExits.end(); ++it, ++i) {
		if(!((*it)->isHidden())) {
			s << (*it)->getName();
			// only add a space if we have more exits after this one
			if(i < numExits - 1) {
				s << " ";
			}
		}
	}

	return s.str();
}

/// takes care of any special messages
/** This function allows a Room to listen for a specific message and act accordingly
	@param message a shared_ptr of the message to process
*/
void Room::processMessage(Message::MessagePointer message) {
	resendMessage(message);
}

/// removes a stale player pointer from the room
/** This function removes a player that has logged out or otherwise disappeared
	from the room
*/
void Room::removePlayer(const std::string &name) {
	if(lock()) {
		containerRemove(name);
		unlock();
	}
}

/// handles any ongoing needs for the room
/** This function is called every HEARTBEAT_RESOLUTION seconds and can be used to process
	any regular updates that may be needed. It also is responsible for equalizing temperatures
	between objects contained herein.
*/
void Room::heartbeat() {
	if(lock()) {
		containerEqualizeTemp(getTemperature());
		unlock();
	}
}

/// Locks this resource so threads don't fight over it
/** A simple function to limit access to the socket one thread at a time
	\return true if able to lock the mutex
*/
bool Room::lock() {
	bool success;

	if(pthread_mutex_lock(&mBusy) != 0) {
		success = false;
		glob.log.error("Room::lock(): Could not lock mutex");
	} else {
		success = true;
	}

	return success;
}

/// Unlocks this resource so other threads can use it
/** A simple function to unlock the mutex and allow another thread access to
	this resource
	\return true if able to unlock the mutex
	\note I have never seen this return false
*/
bool Room::unlock() {
	bool success;

	if(pthread_mutex_unlock(&mBusy) != 0) {
		success = false;
		glob.log.error("Room::unlock(): Could not unlock mutex");
	} else {
		success = true;
	}

	return success;
}

/// sets the weather for the room
/** This function handles changing the room's weather, if available. It should
	not be changed directly, but by the weather map at the zone level.
	@param weather A char representing the weather
	@param wind How strong the wind is blowing
	@param direction Which direction the wind is blowing
	\note A message will be sent to occupants of the room if it is not empty
*/
void Room::setWeather(const char weather, const int wind, Direction direction) {
	// don't send out notifications if the room is empty
	if(isEmpty()) {
		mCurrentWeather = weather;
		mWindStrength = wind;
		mWindDirection = direction;
		return;
	}

	Zone::ZonePointer zone = glob.zoneDaemon.getZone(mZoneName);

	if(!zone) {
		glob.log.error(boost::format("Room::setWeather(): Zone %1% cannot be found") % mZoneName);
		return;
	}

	std::stringstream s;

	bool needSpace = false;

	if(mCurrentWeather != weather) {
		mCurrentWeather = weather;
		s << zone->getWeatherForRoom(mMapX, mMapY);
		needSpace = true;
	}

	if(mWindStrength != wind || mWindDirection != direction) {
		mWindStrength = wind;
		mWindDirection = direction;
		if(needSpace) {
			s << " ";
		}
		s << zone->getWindString();
	}

	if(s.str().empty()) {
		// no change in weather
		return;
	}

	Message::MessagePointer msg = Message::MessagePointer(new Message);
	msg->setType(Message::Information);
	msg->setFrom("Weather");
	msg->setBody(s.str());

	processMessage(msg);
}
