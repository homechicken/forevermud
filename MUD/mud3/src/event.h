#ifndef MUD_EVENT_H
#define MUD_EVENT_H

#include "mudconfig.h"
#include <string>
#include <vector>

/// a class to handle timed events
/** This class defines an event that should be fired off at a later time
*/
class Event {
public:
	Event();
	~Event();

	void decrement();

	/// find out how much time is left before this event goes off
	int getTimeLeft() const { return mTimeLeft; }

	/// set how long before this event goes off, in \b heartbeats
	void setTimeLeft(const int set) { mTimeLeft = set; }

	/// gets the name of this event
	std::string getEventName() const { return mEventName; }

	/// sets the name of this event
	void setEventName(const std::string &set) { mEventName = set; }

	/// gets the name of the target
	std::string getTarget() const { return mTarget; }

	/// sets the name of the target
	void setTarget(const std::string &set) { mTarget = set; }

	/// gets the name of the target zone
	std::string getTargetZone() const { return mTargetZone; }

	/// sets the name of the target zone
	void setTargetZone(const std::string &zone)	{ mTargetZone = zone; }

	/// sets the type of object that the target is
	void setTargetObjectType(ObjectType type) { mTargetObjectType = type; }

	/// gets the type of the target object
	ObjectType getTargetObjectType() const { return mTargetObjectType; }

	/// add arguments for the event when it goes off
	void setArguments(const std::vector<std::string> &set) { mArguments = set; }

	/// get the list of arguments for this event
	std::vector<std::string> getArguments() const { return mArguments; }

private:
	int mTimeLeft;	///< How much time is left before the event takes place
	std::string mEventName;	///< What the event name is
	std::string mTarget;	///< To whom the event should happen
	std::string mTargetZone;	///< If the target is a room, we need this set too
	ObjectType mTargetObjectType;	///<  What kind of object is the target?
	std::vector<std::string> mArguments;	///< Any arguments that should be included when the event is fired off
};

#endif // MUD_EVENT_H
