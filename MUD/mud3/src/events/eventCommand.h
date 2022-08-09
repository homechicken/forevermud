#ifndef EVENTCOMMAND_H
#define EVENTCOMMAND_H

#include <string>
#include <vector>

#include "mudconfig.h"

/// a basic prototype of all event commands
/** This class defines how an event command should behave. All event commands should
	be derived from this class.
*/
class EventCommand {
public:
	/// constructor, does nothing
	EventCommand() {};
	/// destructor, does nothing
	virtual ~EventCommand() {};
	
	/// sets the name of the event
	void setName(const std::string &set) { name = set; }
	/// gets the name of the event
	std::string getName() { return name; }
	
	/// code that is executed when the event timer is reached, pure virtual, must be overridden
	virtual void process(const std::string &target, ObjectType type, const std::vector<std::string> &arguments) = 0;
	
private:
	std::string name;
};

#endif // EVENTCOMMAND_H
