#include <string>
#include <sstream>
#include <boost/cast.hpp>
#include "mudconfig.h"
#include "statEngine.h"

/// Constructor
/** Zeros all statistic variables
*/
StatEngine::StatEngine() {
	mEngineStartTime = time(NULL);
	mBytesOut = 0;
	mBytesIn = 0;
	mLoopTime = 0;
	mNumberOfLoops = 0;
}

/// Destructor
/** Does nothing
*/
StatEngine::~StatEngine() {
}

/// adds to the bytes-in count
/** This function adds to the total number of bytes the server has read in
	@param in number of bytes in this time
*/
void StatEngine::addBytesIn(unsigned long in) {
	mBytesIn += in;
}

/// adds to the bytes-out count
/** This function adds to the total number of bytes the server has written out
	@param out the number of bytes out to add to the count
*/
void StatEngine::addBytesOut(unsigned long out) {
	mBytesOut += out;
}

/// adds to the time the server has slept
/** This function adds to the total time the server has slept and increments the
	total number of loops
	@param sleep the amount of time slept, in microseconds
*/
void StatEngine::addSleepTime(unsigned long sleep) {
	++mNumberOfLoops;
	mLoopTime += sleep;
}

/// How long the process thread stays awake on average
/** This function calculates how long \e on \e average the server stays awake
	\return the average loop processing time
*/
float StatEngine::getAverageLoopProcessTime() {
	float f = boost::numeric_cast<float>(TIME_RESOLUTION - (mLoopTime / mNumberOfLoops));

	return f;
}

/// creates a string of the time the server has been up
/** This function creates a string describing how long the server has been up
	\return a string expressing the server uptime in weeks, days, hours, minutes,
	and seconds
*/
std::string StatEngine::getEngineUptime() {
	std::stringstream s;
	s << "The MUD has been up for " << getTimeDifference(time(NULL), mEngineStartTime);
	return s.str();
}

/// how many seconds the server has been running
/** This function returns the number of seconds that the server has been up and
	running
	\return a time_t value of the number of seconds
*/
time_t StatEngine::getEngineUptimeSeconds() {
	return time(NULL) - mEngineStartTime;
}

/// Takes two times and returns a nice, formatted string of the difference
/** This function creates a string describing the difference between two \c time_t
	objects
	@param later the later of the two \c time_t values
	@param earlier the earlier of the two \c time_t values
	\return a string describing the difference between the two times
*/
std::string StatEngine::getTimeDifference(const time_t later, const time_t earlier) {
	std::stringstream s;
	time_t seconds = later - earlier;

	int weeks = seconds / 604800;
	if(weeks > 0) {
		s << weeks;
		if(weeks == 1) {
			s << " week, ";
		} else {
			s << " weeks, ";
		}
		seconds %= 604800;
	}
	int days = seconds / 86400;
	if(days > 0) {
		s << days;
		if(days == 1) {
			s << " day, ";
		} else {
			s << " days, ";
		}
		seconds %= 86400;
	}
	int hours = seconds / 3600;
	if(hours > 0) {
		s << hours;
		if(hours == 1) {
			s << " hour, ";
		} else {
			s << " hours, ";
		}
		seconds %= 3600;
	}
	int minutes = seconds / 60;
	if(minutes > 0) {
		s << minutes;
		if(minutes == 1) {
			s << " minute, ";
		} else {
			s << " minutes, ";
		}
		seconds %= 60;
	}
	s << seconds;
	if(seconds == 1) {
		s << " second.";
	} else {
		s << " seconds.";
	}

	return s.str();
}
