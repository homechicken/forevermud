#include <sstream>
#include "eventDaemon.h"

#include "global.h"
extern Global glob;

// event headers
#include "eBsotg.h"

/// Constructor
/** Loads all possible events into a map so the code is accessible when a related
	event is fired off.
	\see CommandHandler and loadCommands.cpp for related code
*/
EventDaemon::EventDaemon() {
	loadEvents();
}

/// Destructor
/** Nothing is done here
*/
EventDaemon::~EventDaemon() {
}

/// tells events when to decrement their counter and fires them off when it's time
/** This function is the brains of the daemon: it does all the work. It gets called
	every \b heartbeat, where it goes through all registered Event objects and tells
	them to decrement themselves. It checks for an expired timer and fires off the
	proper command when needed.
*/
void EventDaemon::processEvents() {
	std::vector<Event>::iterator pos;
	std::stringstream s;

	for(pos = mEventList.begin(); pos < mEventList.end(); ++pos) {
		pos->decrement();

		if(pos->getTimeLeft() == 0) {

			glob.log.debug(boost::format("EventDaemon::processEvents(): Event %1% reached") % pos->getEventName());

			Player::PlayerPointer player;
			Zone::ZonePointer zone;
			Room::RoomPointer room;

			ObjectType type = pos->getTargetObjectType();

			switch(type) {
				case RoomObject:
					zone = glob.zoneDaemon.getZone(pos->getTargetZone());

					if(!zone) {
						glob.log.error(boost::format("EventDaemon::processEvents(): Cannot get zone %1% for event") % pos->getTargetZone());
					} else {
						room = zone->getRoom(pos->getTarget());
						if(!room) {
							glob.log.error(boost::format("EventDaemon::processEvents(): Cannot get room %1% from zone %2% for event") % pos->getTarget() % pos->getTargetZone());
							clearEventsForTarget(pos->getTarget());
							return;
						}
					}

					break;

				case PlayerObject: // fall through, events default to players
				default:
					player = glob.playerDatabase.getPlayer(pos->getTarget());

					if(!player) {
						// no such player logged in
						glob.log.error("EventDaemon: no such player logged in");

						// clear all events for player, not just this one...
						clearEventsForTarget(pos->getTarget());
						return;
					}
					break;
			}

			EventCommandMap::iterator eventPos;
			eventPos = mEventCommandMap.find(pos->getEventName());

			if(eventPos == mEventCommandMap.end()) {
				// no such event found!
				glob.log.error("EventDaemon: no such event found");
				mEventList.erase(pos);
				return;
			}

			glob.log.debug(boost::format("EventDaemon::processEvents(): Event triggered for %1%") % pos->getTarget());

			eventPos->second->process(pos->getTarget(), type, pos->getArguments());

			mEventList.erase(pos);
		}
	}
}

/// generates a list of all Event objects for a single person
/** This function makes a list of events targeted at a person. It will probably
	only be called when saving the person's data or when the player's object
	goes out of scope.
	@param name the name of the target to look for
	\return a copy of all Event objects targeted for this player
*/
std::vector<Event> EventDaemon::getEventsForTarget(const std::string &name) const {
	std::vector<Event> myEvents;
	std::vector<Event>::const_iterator pos;

	for(pos = mEventList.begin(); pos < mEventList.end(); ++pos) {
		if(pos->getTarget() == name) {
			myEvents.push_back(*pos);
		}
	}

	return myEvents;
}

/// removes all Event objects targeted at this player
/** This function removes all registered Event objects for a specific plaer.
	@param name the name of the player to clear events for
*/
void EventDaemon::clearEventsForTarget(const std::string &name) {
	std::vector<Event>::iterator pos;
	/// \todo use erase/remove idiom
	for(pos = mEventList.begin(); pos < mEventList.end(); ++pos) {
		if(pos->getTarget() == name) {
			mEventList.erase(pos);
		}
	}
}

/// creates a map of event names to code
/** This is how Event objects are fired off. The name of the Event is matched to
	the right function. Only one copy of each event is stored in memory.
*/
void EventDaemon::loadEvents() {
	mEventCommandMap["bsotg"] = EventCommandPtr(new eBsotg());
}


