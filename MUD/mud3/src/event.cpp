#include "event.h"

/// Constructor
/** Nothing done here
*/
Event::Event() {
}

/// Destructor
/** Nothing done here
*/
Event::~Event() {
}

/// counts down this event
/** This function decrements a counter, keeping it at zero if the time has expired
	for it to be fired off
*/
void Event::decrement() {
	mTimeLeft -= 1;
	if(mTimeLeft < 1) {
		mTimeLeft = 0;
	}
}
