#ifndef MUD_EVENTDAEMON_H
#define MUD_EVENTDAEMON_H

#include "mudconfig.h"
#include "event.h"
#include "eventCommand.h"

#include <map>
#include <boost/shared_ptr.hpp>

/// class to control all events on the server
/** This class keeps track of all Event objects and is in charge of firing them
	off when their internal timers expire
*/
class EventDaemon {
public:
	/// typedef to make writing shared pointers easy
	typedef boost::shared_ptr<EventCommand> EventCommandPtr;
	/// typedef to make managing events easier
	typedef std::map<std::string, EventCommandPtr> EventCommandMap;

	EventDaemon();
	~EventDaemon();

	/// adds an event to the watch list
	void addEvent(const Event &event) { mEventList.push_back(event); }
	void processEvents();

	std::vector<Event> getEventsForTarget(const std::string &name) const;
	void clearEventsForTarget(const std::string &name);

private:
	std::vector<Event> mEventList;	///< a list of events
	EventCommandMap mEventCommandMap;	///< a map to find the code for the event when needed

	void loadEvents();
};

#endif // MUD_EVENTDAEMON_H
