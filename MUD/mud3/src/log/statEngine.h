#ifndef STATENGINE_H
#define STATENGINE_H

/// A statistics-gathering engine
/** This class gathers statistics for the MUD while it is running. Loop processing
	times, bytes in and out, and other data are tracked here. Also contains
	a few functions to retrieve information about these statistics.
*/
class StatEngine {
public:
	StatEngine();
	~StatEngine();
	
	void addBytesOut(unsigned long out);
	void addBytesIn(unsigned long in);

	/// get the number of bytes the server has written out
	unsigned long getBytesOut() { return mBytesOut; }
	/// get the number of bytes the server has read in
	unsigned long getBytesIn() { return mBytesIn; }
	
	void addSleepTime(unsigned long sleep);
	
	std::string getEngineUptime();
	std::string getTimeDifference(const time_t later, const time_t earlier);
	
	time_t getEngineUptimeSeconds();
	
	float getAverageLoopProcessTime();
	
private:
	unsigned long mBytesOut;	///< number of bytes out from the server
	unsigned long mBytesIn;		///< number of bytes in to the server
	time_t mEngineStartTime;	///< time when the server started
	unsigned long long mLoopTime;	///< how much time is spent in loops
	unsigned int mNumberOfLoops;	///< how many loops have happenend
};

#endif // STATENGINE_H
